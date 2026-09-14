#ifndef SCHEMA_HPP
#define SCHEMA_HPP

#include "matador/query/basic_schema.hpp"

#include "matador/query/observer.hpp"
#include "matador/query/relation_completer.hpp"

#include "matador/utils/result.hpp"
#include "matador/utils/error.hpp"

#include <memory>
#include <string>
#include <unordered_set>

namespace matador::query {
/**
 * @brief Type-safe schema registry for persistent object mappings.
 *
 * The `schema` class stores the mapping between C++ types and database table
 * metadata. A type is attached to the schema under a table/node name and can
 * later be queried by type or by name through the inherited `basic_schema`
 * interface.
 *
 * `schema` extends `basic_schema` with template-based helpers that create
 * typed schema nodes, complete relationships between mapped types, and attach
 * optional observers. Relationships declared in mapped model classes are
 * completed automatically when the related types become available.
 *
 * A mapped type is expected to provide the metadata required by Matador's
 * access layer, usually through a `process` function.
 *
 * @code{.cpp}
 * struct person {
 *   unsigned int id{};
 *   std::string name;
 *
 *   template<typename Operator>
 *   void process(Operator& op) {
 *     matador::query::access::primary_key(op, "id", id);
 *     matador::query::access::attribute(op, "name", name);
 *   }
 * };
 *
 * matador::query::schema db;
 *
 * auto result = db.attach<person>("people");
 * if (!result) {
 *   std::cerr << result.err().message() << '\n';
 * }
 * @endcode
 *
 * Inheritance can be represented by attaching a type below an already attached
 * parent type.
 *
 * @code{.cpp}
 * struct user {
 *   virtual ~user() = default;
 *
 *   template<typename Operator>
 *   void process(Operator& op) {
 *     // map base attributes
 *   }
 * };
 *
 * struct admin : user {
 *   template<typename Operator>
 *   void process(Operator& op) {
 *     // map admin attributes
 *   }
 * };
 *
 * matador::query::schema db;
 *
 * auto result = db.attach<user>("users")
 *   .and_then([&db] {
 *     return db.attach<admin, user>("admins");
 *   });
 * @endcode
 *
 * Observers can be passed during attachment to receive lifecycle callbacks for
 * the mapped type.
 *
 * @code{.cpp}
 * template<typename Type>
 * class audit_observer : public matador::query::observer<Type> {
 * public:
 *   void on_attach(const matador::query::schema_node&, const Type&) const override {}
 *   void on_detach(const matador::query::schema_node&, const Type&) const override {}
 *   void on_insert(const Type&) override {}
 *   void on_update(const Type&) override {}
 *   void on_delete(const Type&) override {}
 * };
 *
 * matador::query::schema db;
 * auto result = db.attach<person>("people", audit_observer<person>{});
 * @endcode
 *
 * @see basic_schema
 * @see schema_node
 * @see observer
 * @see relation_completer
 */
class schema : public basic_schema {
public:
  using basic_schema::basic_schema;

  /**
   * @brief Attaches a type as a root schema node.
   *
   * Creates a schema node for `Type` and registers it under `name`. The given
   * name is typically the database table name used for the mapped type.
   *
   * Optional observers can be supplied to observe attach, detach, insert,
   * update, and delete events for objects of `Type`.
   *
   * If the type contains relation declarations, relation metadata is completed
   * after the node has been attached. If a related type is not attached yet,
   * completion may be finalized when that related type is later attached.
   *
   * @tparam Type Object type to attach to the schema.
   * @tparam Observers Observer class templates for `Type`.
   *
   * @param name Name of the schema node/table.
   * @param observers Optional observer instances for `Type`.
   *
   * @return `ok<void>()` on success; otherwise a failure containing an
   *         error that describes why the node could not be attached.
   *
   * @retval error_code::NodeAlreadyExists A node with the same name already
   *         exists, or `Type` is already attached under another non-empty name.
   *
   * @code{.cpp}
   * struct person {
   *   unsigned int id{};
   *   std::string name;
   *
   *   template<typename Operator>
   *   void process(Operator& op) {
   *     matador::query::access::primary_key(op, "id", id);
   *     matador::query::access::attribute(op, "name", name);
   *   }
   * };
   *
   * matador::query::schema db;
   *
   * auto result = db.attach<person>("people");
   * if (result) {
   *   // The type `person` is now available as schema node "people".
   * }
   * @endcode
   */
  template<typename Type, template<typename> typename... Observers>
  [[nodiscard]] result<void, error> attach(const std::string &name, Observers<Type>&&... observers) {
    return attach_type<Type, Observers...>(name, std::string{}, std::forward<Observers<Type>>(observers)...);
  }

  /**
   * @brief Attaches a type below an already attached parent type.
   *
   * Looks up the schema node registered for `SuperType` and attaches `Type`
   * below that node. This overload is useful for schemas that model an
   * inheritance hierarchy.
   *
   * The parent type must already be present in the schema. If it is not found,
   * the method returns a failure and does not attach `Type`.
   *
   * @tparam Type Object type to attach.
   * @tparam SuperType Already attached parent object type.
   * @tparam Observers Observer class templates for `Type`.
   *
   * @param name Name of the schema node/table for `Type`.
   * @param observers Optional observer instances for `Type`.
   *
   * @return `ok<void>()` on success; otherwise a failure containing an
   *         error that describes why the node could not be attached.
   *
   * @retval error_code::NodeNotFound The parent type `SuperType` is not
   *         attached to the schema.
   * @retval error_code::NodeAlreadyExists A node with the same name already
   *         exists, or `Type` is already attached under another non-empty name.
   *
   * @code{.cpp}
   * struct person {
   *   virtual ~person() = default;
   *
   *   template<typename Operator>
   *   void process(Operator& op) {
   *     // map common person fields
   *   }
   * };
   *
   * struct student : person {
   *   template<typename Operator>
   *   void process(Operator& op) {
   *     // map student fields
   *   }
   * };
   *
   * matador::query::schema db;
   *
   * auto result = db.attach<person>("people")
   *   .and_then([&db] {
   *     return db.attach<student, person>("students");
   *   });
   * @endcode
   */
  template<typename Type, typename SuperType, template<typename> typename... Observers>
  [[nodiscard]] result<void, error> attach(const std::string &name, Observers<Type>&&... observers) {
    const auto ti = std::type_index(typeid(SuperType));
    const auto it = find_node(ti);
    if (it == end()) {
      return failure<error>(make_error(error_code::NodeNotFound, "Parent node '" + std::string(ti.name()) + "' not found"));
    }

    return attach_type<Type, Observers...>(name, it->name(), std::forward<Observers<Type>>(observers)...);
  }

  /**
   * @brief Attaches a type below a parent schema node identified by name.
   *
   * Creates or completes the schema node for `Type` and attaches it below the
   * existing parent node named by `parent`. Passing an empty parent name attaches
   * the type as a root node.
   *
   * This overload is useful when the parent node is known by schema/table name
   * instead of by C++ type.
   *
   * @tparam Type Object type to attach.
   * @tparam Observers Observer class templates for `Type`.
   *
   * @param name Name of the schema node/table for `Type`.
   * @param parent Name of the parent schema node. Use an empty string for a
   *        root node.
   * @param observers Optional observer instances for `Type`.
   *
   * @return `ok<void>()` on success; otherwise a failure containing an
   *         error that describes why the node could not be attached.
   *
   * @retval error_code::NodeNotFound The named parent node does not exist.
   * @retval error_code::NodeAlreadyExists A node with the same name already
   *         exists, or `Type` is already attached under another non-empty name.
   *
   * @code{.cpp}
   * matador::query::schema db;
   *
   * auto result = db.attach<person>("people")
   *   .and_then([&db] {
   *     return db.attach<student>("students", "people");
   *   });
   * @endcode
   */
  template<typename Type, template<typename> typename... Observers>
  [[nodiscard]] result<void, error> attach(const std::string &name, const std::string &parent, Observers<Type>&&... observers) {
    return attach_type<Type, Observers...>(name, parent, std::forward<Observers<Type>>(observers)...);
  }

  /**
   * @brief Implementation helper used by all typed attach overloads.
   *
   * Registers `Type` under `name`, attaches it below `parent`, stores any
   * provided observers, and completes relation metadata for the type.
   *
   * If the type has already been announced internally by relation completion,
   * the announced node is reused and assigned the supplied name. If the type is
   * already known but still has an empty name, this method names and completes
   * that node. If the type is already attached with a non-empty name, attaching
   * it again fails.
   *
   * Most user code should call one of the public `attach()` overloads instead
   * of calling this method directly.
   *
   * @tparam Type Object type to attach.
   * @tparam Observers Observer class templates for `Type`.
   *
   * @param name Name of the schema node/table for `Type`.
   * @param parent Name of the parent schema node. Use an empty string for a
   *        root node.
   * @param observers Optional observer instances for `Type`.
   *
   * @return `ok<void>()` on success; otherwise a failure containing an
   *         error from node attachment or relation completion.
   *
   * @retval error_code::NodeAlreadyExists A node with the same name already
   *         exists, or `Type` is already attached under another non-empty name.
   * @retval error_code::NodeNotFound The named parent node does not exist.
   *
   * @code{.cpp}
   * matador::query::schema db;
   *
   * // Equivalent to db.attach<person>("people").
   * auto result = db.attach_type<person>("people", "");
   * @endcode
   */
  template<typename Type, template<typename> typename... Observers>
  [[nodiscard]] result<void, error> attach_type(const std::string &name, const std::string &parent, Observers<Type>&&... observers) {
    const std::type_index ti{typeid(Type)};

    if (const auto it = nodes_by_type_.find(ti); it == nodes_by_type_.end() ) {
      std::vector<std::unique_ptr<observer<Type>>> obs;
      obs.reserve(sizeof...(Observers));
      (obs.push_back(std::make_unique<Observers<Type>>(std::forward<Observers<Type>>(observers))), ...);

      // if the type was not found
      std::unique_ptr<schema_node> node;
      if (is_node_announced(ti)) {
        node = pop_announce_node(ti);
        node->update_name(name);
      } else {
        node = schema_node::make_node<Type>(*this, name, []{ return std::make_unique<Type>(); }, std::move(obs));
      }

      auto result = attach_node(std::move(node), parent);
      if (!result) {
        return failure<error>(result.err());
      }

      schema_node* attached_node = result.value();
      const auto info = attached_node->template info<Type>();
      auto completer_result = relation_completer<Type, Observers...>::complete(attached_node, info.get().observers());
      if (!completer_result) {
        return failure<error>(completer_result.err());
      }
    } else if (!has_node(name)) {
      const auto old_name = it->second->name();

      if (!old_name.empty()) {
        return failure<error>(make_error(
          error_code::NodeAlreadyExists,
          "Type '" + std::string(ti.name()) + "' is already attached as node '" + old_name + "'"
        ));
      }

      it->second->update_name(name);
      nodes_by_name_.erase(old_name);
      nodes_by_name_[name] = it->second;

      const auto info = it->second->info<Type>();
      auto completer_result = relation_completer<Type, Observers...>::complete(it->second, info.get().observers());
      if (!completer_result) {
        return failure<error>(completer_result.err());
      }
      // log_.info("attach: update node name to '%s' (type: %s)", it->second->name().c_str(), it->second->type_index().name());
    } else {
      return failure<error>(make_error(error_code::NodeAlreadyExists, "Node '" + name + "' already exists"));
    }

    return ok<void>();
  }

  /**
   * @brief Returns typed table/object metadata for an attached type.
   *
   * Looks up the schema node for `Type` and returns its typed object metadata.
   * The returned reference can be used to inspect information discovered for
   * the mapped type, such as its table representation, fields, columns, and
   * relation endpoints.
   *
   * @tparam Type Object type whose metadata should be returned.
   *
   * @return `ok(object_info_ref<Type>)` when `Type` is attached;
   *         otherwise a failure containing an error.
   *
   * @retval error_code::NodeNotFound `Type` is not attached to the schema.
   *
   * @code{.cpp}
   * matador::query::schema db;
   *
   * auto attached = db.attach<person>("people");
   * if (!attached) {
   *   std::cerr << attached.err().message() << '\n';
   *   return;
   * }
   *
   * auto info = db.info<person>();
   * if (info) {
   *   const auto& person_info = info.value().get();
   *   // Inspect metadata for `person`.
   * }
   * @endcode
   */
  template<typename Type>
  [[nodiscard]] result<object_info_ref<Type>, error> info() const {
    const auto it = find_node(std::type_index(typeid(Type)));
    if (it == end()) {
      return failure<error>(make_error(error_code::NodeNotFound, "Parent node '" + std::string(typeid(Type).name()) + "' not found"));
    }

    return ok<object_info_ref<Type>>(it->info<Type>());
  }
};
}

namespace matador {
using schema = query::schema;
}
#endif //SCHEMA_HPP
