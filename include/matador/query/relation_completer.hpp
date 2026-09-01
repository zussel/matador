#ifndef RELATION_COMPLETER_HPP
#define RELATION_COMPLETER_HPP

#include "matador/query/basic_schema.hpp"
#include "matador/query/internal/observer_list_copy_creator.hpp"
#include "matador/query/many_to_many_relation.hpp"
#include "matador/query/join_columns_collector.hpp"
#include "matador/query/schema_node.hpp"

// #include "matador/logger/log_manager.hpp"

#include "matador/query/primary_key_options.hpp"
#include "matador/utils/error_exception.hpp"

#include <functional>
#include <typeindex>
#include <utility>
#include <vector>

namespace matador::query {

class join_column_finder final {
public:
    template<typename Type>
    static bool has_join_column(const std::string &join_column) {
        join_column_finder finder(join_column);
        Type obj;
        finder.found_ = false;
        field::process(finder, obj);
        return finder.found_;
    }

    template<typename BaseType>
    static void on_base(const BaseType&) {}
    template<class PrimaryKeyType>
    static void on_primary_key(const char * /*id*/, PrimaryKeyType &/*pk*/, const primary_key_options& /*attr*/) {}
    static void on_revision(const char * /*id*/, uint64_t &/*rev*/) {}
    template<typename AttributeType>
    static void on_attribute(const char * /*id*/, AttributeType &/*val*/, const column_options &/*attr*/) {}
    template<typename AttributeType>
    static void on_attribute(const char * /*id*/, std::optional<AttributeType> &/*val*/, const column_options &/*attr*/) {}

    template<class ForeignPointerType>
    void on_belongs_to(const char *id, ForeignPointerType &/*obj*/, const foreign_options &/*attr*/) {
      found_ = found_ || requested_join_column_ == id;
    }
    template<class ForeignPointerType>
    static void on_has_one(const char * /*id*/, ForeignPointerType &/*obj*/, const char * /*join_column*/, const foreign_options &/*attr*/) {}

    template<class CollectionType>
    static void on_has_many(const char * /*id*/, CollectionType &, const char * /*join_column*/, const foreign_options &/*attr*/) {}

    template<class CollectionType>
    static void on_has_many_to_many(const char * /*id*/, CollectionType &/*collection*/, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_options &/*attr*/) {}
    template<class CollectionType>
    static void on_has_many_to_many(const char * /*id*/, CollectionType & /*collection*/, const foreign_options &/*attr*/) {}

private:
    explicit join_column_finder(std::string join_column)
    : requested_join_column_(std::move(join_column)) {}

private:
    std::string requested_join_column_;
    bool found_ = false;
};
/*
 *   1. has_many (MM)
 *      no belongs to
 *      relation table is needed
 *      - element type is a foreign table (FT),
 *        then relation table must look like follows:
 *        relation_table<MM, FT>
 *        where MM and FT must be defined as belongs to
 *      - element type if a builtin type BT (i.e. string, int, etc.),
 *        then the relation table must look like follows:
 *        relation_table<MM, BT>
 *        where MM as belongs to and BT as given type
 *
 *   2. has_many_to_many (MM1, MM2)
 *      relation_table is needed
 *      relation_table<MM1, MM2>
 *      where MM1 and MM2 must be defined as belongs to
 *
 *   3. hans_many (MM) <-> belongs_to (BT)
 *      belongs_to has foreign key to the has_many side
 *      no relation table needed
 *
 *   4. has_one to belongs_to
 *      no relation table is needed
 *
 *   5. has_many (MM) <-> has_one (HO)
 *      invalid relation -> error
 *
 *   6. has_one
 *      no has_many or belongs_to
 *      invalid relation -> error
 */
template<typename Type, template<typename> typename... Observers>
class relation_completer final {
public:
  using endpoint_ptr = std::shared_ptr<relation_endpoint>;

  static utils::result<void, utils::error> complete(schema_node *node, const std::vector<std::unique_ptr<observer<Type>>> &observers) {
    relation_completer completer(node->repo_, observers);

    return completer.complete_node_relations(node);
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template<class PrimaryKeyType>
  static void on_primary_key(const char * /*id*/, PrimaryKeyType &/*pk*/, const primary_key_options& /*attr*/) {}
  static void on_revision(const char * /*id*/, uint64_t &/*rev*/) {}
  template<typename AttributeType>
  static void on_attribute(const char * /*id*/, AttributeType &/*val*/, const column_options &/*attr*/) {}
  template<typename AttributeType>
  static void on_attribute(const char * /*id*/, std::optional<AttributeType> &/*val*/, const column_options &/*attr*/) {}

  template<class ForeignPointerType>
  void on_belongs_to(const char *id, ForeignPointerType &obj, const foreign_options &attr);
  template<class ForeignPointerType>
  void on_has_one(const char * /*id*/, ForeignPointerType &/*obj*/, const char * /*join_column*/, const foreign_options &/*attr*/);

  template<class CollectionType>
  void on_has_many(const char *id, CollectionType &, const char *join_column, const foreign_options &attr, std::enable_if_t<is_object_ptr<typename CollectionType::value_type>::value> * = nullptr);
  template<class CollectionType>
  void on_has_many(const char *id, CollectionType &, const char *join_column, const foreign_options &attr, std::enable_if_t<!is_object_ptr<typename CollectionType::value_type>::value> * = nullptr);

  template<class CollectionType>
  void on_has_many_to_many(const char *id, CollectionType &collection, const char *join_column, const char *inverse_join_column, const foreign_options &attr);
  template<class CollectionType>
  void on_has_many_to_many(const char *id, CollectionType &collection, const foreign_options &attr);

private:
  using resolve_action = std::function<void()>;

  explicit relation_completer(basic_schema &repo, const std::vector<std::unique_ptr<observer<Type>>>& observers)
  : repo_(repo)
  // , log_(logger::create_logger("relation_completer"))
  , observers_(observers) {}

  utils::result<void, utils::error> complete_node_relations(schema_node* node) {
    try {
      current_node_ = node;

      Type obj;
      field::process(*this, obj);

      for (auto &action : resolve_actions_) {
        action();
      }

      current_node_ = nullptr;
    } catch (const error_exception &e) {
      current_node_ = nullptr;
      return utils::failure(e.error());
    }

    return utils::ok<void>();
  }

  template<class CollectionType>
  void resolve_object_has_many(std::string id, std::string join_column);

  template<class CollectionType>
  void resolve_value_has_many(std::string id, std::string join_column);

  template<class CollectionType>
  void resolve_has_many_to_many(std::string id, std::string join_column, std::string inverse_join_column);

  template<class ForeignPointerType>
  void resolve_has_one(std::string id) const;

  template<class ForeignPointerType>
  void resolve_belongs_to(std::string id) const;

  template<typename ForeignType>
  void ensure_foreign_node_announced();

  template<typename NodeType>
  void attach_relation_node(const std::string &name, const std::string &join_column, const std::string &inverse_join_column);

  static void register_relation_endpoints(const endpoint_ptr &endpoint,
                                          const endpoint_ptr &other_endpoint);
  static void link_relation_endpoints(const endpoint_ptr &endpoint,
                                      const endpoint_ptr &other_endpoint);

  [[nodiscard]] const schema_node *resolve_node(const std::type_index &ti) const;
  [[nodiscard]] const schema_node& require_node(const std::type_index &ti, const std::string &message) const;


private:
  schema_node *current_node_{nullptr};
  std::vector<resolve_action> resolve_actions_;
  basic_schema &repo_;
  // logger::logger log_;
  join_columns_collector join_columns_collector_{};
  const std::vector<std::unique_ptr<observer<Type>>>& observers_;
};

template<typename Type, template<typename> typename... Observers>
template<class CollectionType>
void relation_completer<Type, Observers...>::on_has_many(const char *id, CollectionType &,
                                           const char *join_column,
                                           const foreign_options &,
                                           std::enable_if_t<is_object_ptr<typename CollectionType::value_type>::value> * /*unused*/) {
  ensure_foreign_node_announced<typename CollectionType::value_type::value_type>();

  resolve_actions_.push_back([this, id = std::string(id), join_column = std::string(join_column)] {
    resolve_object_has_many<CollectionType>(id, join_column);
  });
}

template<typename Type, template<typename> typename... Observers>
template<class CollectionType>
void relation_completer<Type, Observers...>::on_has_many(const char *id, CollectionType &, const char *join_column,
                                           const foreign_options &,
                                           std::enable_if_t<!is_object_ptr<typename CollectionType::value_type>::value>* /*unused*/) {
  resolve_actions_.push_back([this, id = std::string(id), join_column = std::string(join_column)] {
    resolve_value_has_many<CollectionType>(id, join_column);
  });
}

template<typename Type, template<typename> typename... Observers>
template<class CollectionType>
void relation_completer<Type, Observers...>::on_has_many_to_many(const char *id,
                                                   CollectionType &/*collection*/,
                                                   const char *join_column,
                                                   const char *inverse_join_column,
                                                   const foreign_options &/*attr*/) {
  ensure_foreign_node_announced<typename CollectionType::value_type::value_type>();

  resolve_actions_.push_back([
    this,
    id = std::string(id),
    join_column = std::string(join_column),
    inverse_join_column = std::string(inverse_join_column)
  ] {
    resolve_has_many_to_many<CollectionType>(id, join_column, inverse_join_column);
  });
}

template<typename Type, template<typename> typename... Observers>
template<class CollectionType>
void relation_completer<Type, Observers...>::on_has_many_to_many(const char *id,
                                                   CollectionType &collection,
                                                   const foreign_options &attr) {
  const auto join_columns = join_columns_collector_.collect<typename CollectionType::value_type::value_type>();
  on_has_many_to_many(
    id,
    collection,
    join_columns.inverse_join_column.c_str(),
    join_columns.join_column.c_str(),
    attr);
}

template<typename Type, template<typename> typename... Observers>
template<class ForeignPointerType>
void relation_completer<Type, Observers...>::on_has_one(const char *id,
                                          ForeignPointerType &/*obj*/,
                                          const char * /*join_column*/,
                                          const foreign_options &/*attr*/) {
  ensure_foreign_node_announced<typename ForeignPointerType::value_type>();

  resolve_actions_.push_back([this, id = std::string(id)] {
    resolve_has_one<ForeignPointerType>(id);
  });
}

template<typename Type, template<typename> typename... Observers>
template<class ForeignPointerType>
void relation_completer<Type, Observers...>::on_belongs_to(const char *id,
                                             ForeignPointerType & /*obj*/,
                                             const foreign_options & /*attr*/) {
  ensure_foreign_node_announced<typename ForeignPointerType::value_type>();

  resolve_actions_.push_back([this, id = std::string(id)] {
    resolve_belongs_to<ForeignPointerType>(id);
  });
}

template<typename Type, template<typename> typename... Observers>
template<class CollectionType>
void relation_completer<Type, Observers...>::resolve_object_has_many(std::string id, std::string join_column) {
  using value_type = typename CollectionType::value_type::value_type;
  using relation_value_type = many_to_many_relation<Type, value_type>;

  auto &foreign_node = require_node(
    typeid(value_type),
    "relation completer: foreign node not found"
  );

  if (const auto it = foreign_node.info_->find_relation_endpoint(typeid(Type)); it != foreign_node.info().endpoint_end()) {
    if (!it->second->is_belongs_to()) {
      throw error_exception(error_code::InvalidArgument, "relation completer: has_many can only be linked to belongs_to");
    }

    const auto local_endpoint = std::make_shared<relation_endpoint>(id, relation_type::HasMany, foreign_node);
    current_node_->info_->register_relation_endpoint(typeid(value_type), local_endpoint);
    link_relation_endpoints(local_endpoint, it->second);
    return;
  }

  if (join_column_finder::has_join_column<value_type>(join_column)) {
    const auto local_endpoint = std::make_shared<relation_endpoint>(id, relation_type::HasMany, foreign_node);
    current_node_->info_->register_relation_endpoint(typeid(value_type), local_endpoint);
    return;
  }

  // log_.debug("node '%s' has many foreign keys '%s' mapped by '%s'", current_node_->name().c_str(), id.c_str(), join_column.c_str());

  auto node = schema_node::make_node<relation_value_type>(repo_, id, [join_column] {
    return std::make_unique<relation_value_type>("id", join_column);
  }, {});
  auto result = repo_.attach_node(std::move(node), "");
  if (!result) {
    throw error_exception(result.release_error());
  }

  auto *attached_node = result.value();
  const auto local_endpoint = std::make_shared<relation_endpoint>(id, relation_type::HasMany, *attached_node);
  const auto foreign_endpoint = std::make_shared<relation_endpoint>("id", relation_type::BelongsTo, *current_node_);

  current_node_->info_->register_relation_endpoint(typeid(value_type), local_endpoint);
  attached_node->info_->register_relation_endpoint(current_node_->type_index(), foreign_endpoint);
  link_relation_endpoints(local_endpoint, foreign_endpoint);

  const auto foreign_value_endpoint = std::make_shared<relation_endpoint>(join_column, relation_type::BelongsTo, foreign_node);
  attached_node->info_->register_relation_endpoint(typeid(value_type), foreign_value_endpoint);
}

template<typename Type, template<typename> typename... Observers>
template<class CollectionType>
void relation_completer<Type, Observers...>::resolve_value_has_many(std::string id, std::string join_column) {
  using value_type = typename CollectionType::value_type;
  using relation_value_type = many_to_relation<Type, value_type>;

  auto observers = internal::observer_list_copy_creator<Type, relation_value_type, Observers...>::copy_create(observers_);

  auto node = schema_node::make_node<relation_value_type>(repo_, id, [join_column] {
    return std::make_unique<relation_value_type>(join_column, "value");
  }, std::move(observers));
  auto result = repo_.attach_node(std::move(node), "");
  if (!result) {
    throw error_exception(result.release_error());
  }

  auto *attached_node = result.value();
  const auto local_endpoint = std::make_shared<relation_endpoint>(id, relation_type::HasMany, *attached_node);
  const auto foreign_endpoint = std::make_shared<relation_endpoint>(join_column, relation_type::BelongsTo, *current_node_);

  current_node_->info_->register_relation_endpoint(typeid(value_type), local_endpoint);
  attached_node->info_->register_relation_endpoint(current_node_->type_index(), foreign_endpoint);
  link_relation_endpoints(local_endpoint, foreign_endpoint);
}

template<typename Type, template<typename> typename... Observers>
template<class CollectionType>
void relation_completer<Type, Observers...>::resolve_has_many_to_many(std::string id, std::string join_column, std::string inverse_join_column) {
  using value_type = typename CollectionType::value_type::value_type;

  if (!repo_.expecting_relation_node(id)) {
    repo_.expect_relation_node(id, typeid(value_type));
    return;
  }

  attach_relation_node<value_type>(id, join_column, inverse_join_column);
  repo_.remove_expected_relation_node(id);
}

template<typename Type, template<typename> typename... Observers>
template<class ForeignPointerType>
void relation_completer<Type, Observers...>::resolve_has_one(std::string id) const {
  using value_type = typename ForeignPointerType::value_type;

  auto &foreign_node = require_node(
    typeid(value_type),
    "relation completer: foreign node not found"
  );

  auto local_it = current_node_->info().find_relation_endpoint(typeid(value_type));
  if (local_it == current_node_->info().endpoint_end()) {
    const auto local_endpoint = std::make_shared<relation_endpoint>(id, relation_type::HasOne, foreign_node);
    local_it = current_node_->info_->register_relation_endpoint(typeid(value_type), local_endpoint);
  }

  if (const auto foreign_it = foreign_node.info().find_relation_endpoint(typeid(Type)); foreign_it != foreign_node.info().endpoint_end()) {
    if (!foreign_it->second->is_belongs_to()) {
      throw error_exception(error_code::InvalidArgument, "relation completer: has_one can only be linked to belongs_to");
    }

    link_relation_endpoints(local_it->second, foreign_it->second);
  }
}

template<typename Type, template<typename> typename... Observers>
template<class ForeignPointerType>
void relation_completer<Type, Observers...>::resolve_belongs_to(std::string id) const {
  using value_type = typename ForeignPointerType::value_type;

  const auto ti = std::type_index(typeid(value_type));
  auto &foreign_node = require_node(
    ti,
    "relation completer: foreign node not found"
  );

  if (const auto it = foreign_node.info_->find_relation_endpoint(current_node_->type_index()); it != foreign_node.info().endpoint_end()) {
    if (it->second->is_has_one() || it->second->is_has_many()) {
      const auto endpoint = std::make_shared<relation_endpoint>(id, relation_type::BelongsTo, foreign_node);
      current_node_->info_->register_relation_endpoint(ti, endpoint);
      link_relation_endpoints(endpoint, it->second);
      return;
    }

    const auto foreign_endpoint = it->second->foreign_endpoint();
    if (!foreign_endpoint) {
      throw error_exception(error_code::Failure, "relation completer: relation endpoint is not linked");
    }

    if (foreign_endpoint->node().type_index() != typeid(many_to_many_relation<Type, value_type>)) {
      throw error_exception(error_code::InvalidArgument, "relation completer: incompatible relation endpoint");
    }

    if (auto detach_result = repo_.detach(foreign_endpoint->node_); !detach_result) {
      throw error_exception(detach_result.release_error());
    }

    foreign_endpoint->node_ = &foreign_node;
    current_node_->info_->register_relation_endpoint(ti, foreign_endpoint);
    return;
  }

  const auto endpoint = std::make_shared<relation_endpoint>(id, relation_type::BelongsTo, foreign_node);
  current_node_->info_->register_relation_endpoint(ti, endpoint);
}

template<typename Type, template<typename> typename... Observers>
template<typename ForeignType>
void relation_completer<Type, Observers...>::ensure_foreign_node_announced() {
  const auto foreign_type = std::type_index(typeid(ForeignType));

  if (repo_.contains(foreign_type)) {
    return;
  }

  if (repo_.is_node_announced(foreign_type)) {
    return;
  }

  auto observers = internal::observer_list_copy_creator<Type, ForeignType, Observers...>::copy_create(observers_);

  auto node = schema_node::make_node<ForeignType>(
    repo_,
    "",
    [] { return std::make_unique<ForeignType>(); },
    std::move(observers)
  );

  repo_.push_announce_node(foreign_type, std::move(node));
}

template<typename Type, template<typename> typename... Observers>
template<typename NodeType>
void relation_completer<Type, Observers...>::attach_relation_node(const std::string &name, const std::string &join_column, const std::string &inverse_join_column) {
  using relation_value_type = many_to_many_relation<Type, NodeType>;
  using value_type = NodeType;

  auto &foreign_node = require_node(
    typeid(value_type),
    "relation completer: foreign node not found"
  );

  if (const auto it = repo_.find_node(name); it != repo_.end()) {
    return;
  }

  auto creator = [join_column, inverse_join_column] {
    return std::make_unique<relation_value_type>(join_column, inverse_join_column);
  };

  auto observers = internal::observer_list_copy_creator<Type, relation_value_type, Observers...>::copy_create(observers_);

  auto node = schema_node::make_relation_node<relation_value_type>(repo_,
                                                                       name,
                                                                       join_column,
                                                                       inverse_join_column,
                                                                       std::move(creator),
                                                                       std::move(observers));
  auto result = repo_.attach_node(std::move(node), "");
  if (!result) {
    throw error_exception(result.release_error());
  }

  auto *attached_node = result.value();

  const auto local_endpoint = std::make_shared<relation_endpoint>(name, relation_type::HasMany, *attached_node);
  const auto join_endpoint = std::make_shared<relation_endpoint>(join_column, relation_type::BelongsTo, *current_node_);
  const auto inverse_join_endpoint = std::make_shared<relation_endpoint>(inverse_join_column, relation_type::BelongsTo, foreign_node);
  const auto foreign_endpoint = std::make_shared<relation_endpoint>(name, relation_type::HasMany, *attached_node);

  current_node_->info_->register_relation_endpoint(typeid(value_type), local_endpoint);
  foreign_node.info_->register_relation_endpoint(current_node_->type_index(), foreign_endpoint);

  attached_node->info_->register_relation_endpoint(current_node_->type_index(), join_endpoint);
  attached_node->info_->register_relation_endpoint(typeid(value_type), inverse_join_endpoint);

  link_relation_endpoints(local_endpoint, join_endpoint);
  link_relation_endpoints(foreign_endpoint, inverse_join_endpoint);
}

template<typename Type, template<typename> typename... Observers>
void relation_completer<Type, Observers...>::register_relation_endpoints(const endpoint_ptr &endpoint,
                                                           const endpoint_ptr &other_endpoint) {
  endpoint->node().info_->register_relation_endpoint(other_endpoint->node().type_index(), endpoint);
  other_endpoint->node().info_->register_relation_endpoint(endpoint->node().type_index(), other_endpoint);
  link_relation_endpoints(endpoint, other_endpoint);
}

template<typename Type, template<typename> typename... Observers>
void relation_completer<Type, Observers...>::link_relation_endpoints(const endpoint_ptr &endpoint, const endpoint_ptr &other_endpoint) {
  endpoint->link_foreign_endpoint(other_endpoint);
  other_endpoint->link_foreign_endpoint(endpoint);
}

template<typename Type, template <typename> class ... Observers>
const schema_node *relation_completer<Type, Observers...>::resolve_node(const std::type_index &ti) const {
  if (const auto it = repo_.find_node(ti); it != repo_.end()) {
    return it.get();
  }

  if (!repo_.is_node_announced(ti)) {
    return nullptr;
  }

  return repo_.announce_node(ti);
}

template<typename Type, template <typename> class ... Observers>
const schema_node& relation_completer<Type, Observers...>::require_node(const std::type_index &ti, const std::string &message) const {
  auto *node = resolve_node(ti);
  if (!node) {
    throw error_exception(error_code::NodeNotFound, std::string(message));
  }

  return *node;
}
}
#endif //RELATION_COMPLETER_HPP
