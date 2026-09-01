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
class schema : public basic_schema {
public:
  using basic_schema::basic_schema;

  template<typename Type, template<typename> typename... Observers>
  [[nodiscard]] utils::result<void, utils::error> attach(const std::string &name, Observers<Type>&&... observers) {
    return attach_type<Type, Observers...>(name, std::string{}, std::forward<Observers<Type>>(observers)...);
  }

  template<typename Type, typename SuperType, template<typename> typename... Observers>
  [[nodiscard]] utils::result<void, utils::error> attach(const std::string &name, Observers<Type>&&... observers) {
    const auto ti = std::type_index(typeid(SuperType));
    const auto it = find_node(ti);
    if (it == end()) {
      return utils::failure(make_error(error_code::NodeNotFound, "Parent node '" + std::string(ti.name()) + "' not found"));
    }

    return attach_type<Type, Observers...>(name, it->name(), std::forward<Observers<Type>>(observers)...);
  }

  template<typename Type, template<typename> typename... Observers>
  [[nodiscard]] utils::result<void, utils::error> attach(const std::string &name, const std::string &parent, Observers<Type>&&... observers) {
    return attach_type<Type, Observers...>(name, parent, std::forward<Observers<Type>>(observers)...);
  }
  template<typename Type, template<typename> typename... Observers>
  [[nodiscard]] utils::result<void, utils::error> attach_type(const std::string &name, const std::string &parent, Observers<Type>&&... observers) {
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
        return utils::failure(result.err());
      }

      schema_node* attached_node = result.value();
      const auto info = attached_node->template info<Type>();
      auto completer_result = relation_completer<Type, Observers...>::complete(attached_node, info.get().observers());
      if (!completer_result) {
        return utils::failure(completer_result.err());
      }
    } else if (!has_node(name)) {
      const auto old_name = it->second->name();

      if (!old_name.empty()) {
        return utils::failure(make_error(
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
        return utils::failure(completer_result.err());
      }
      // log_.info("attach: update node name to '%s' (type: %s)", it->second->name().c_str(), it->second->type_index().name());
    } else {
      return utils::failure(make_error(error_code::NodeAlreadyExists, "Node '" + name + "' already exists"));
    }

    return utils::ok<void>();
  }

  template<typename Type>
  [[nodiscard]] utils::result<object_info_ref<Type>, utils::error> info() const {
    const auto it = find_node(std::type_index(typeid(Type)));
    if (it == end()) {
      return utils::failure(make_error(error_code::NodeNotFound, "Parent node '" + std::string(typeid(Type).name()) + "' not found"));
    }

    return utils::ok(it->info<Type>());
  }
};
}

#endif //SCHEMA_HPP
