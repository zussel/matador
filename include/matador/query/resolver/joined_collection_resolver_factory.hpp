#ifndef MATADOR_CONTAINER_RESOLVER_FACTORY_HPP
#define MATADOR_CONTAINER_RESOLVER_FACTORY_HPP

#include "matador/query/collection_resolver.hpp"

#include <memory>

namespace matador::query {
class abstract_collection_resolver_factory {
public:
  virtual ~abstract_collection_resolver_factory() = default;

  [[nodiscard]] virtual std::shared_ptr<abstract_joined_resolver> acquire_collection_resolver(const std::type_index &root_type, const std::type_index &element_type, const std::string &collection_name) const = 0;
  virtual void register_collection_resolver(std::shared_ptr<abstract_joined_resolver> &&resolver) = 0;
};

class joined_collection_resolver_factory : public abstract_collection_resolver_factory {
public:
  template<class Type>
  [[nodiscard]] std::shared_ptr<collection_resolver<Type>> resolver(const std::type_index &root_type, const std::string &collection_name) const {
    const auto res = acquire_collection_resolver(root_type, typeid(Type), collection_name);
    if (!res) {
      return std::dynamic_pointer_cast<collection_resolver<Type>>(res);
    }

    return std::dynamic_pointer_cast<collection_resolver<Type>>(res);
  }
};

class abstract_joined_object_resolver_factory {
public:
  virtual ~abstract_joined_object_resolver_factory() = default;

  [[nodiscard]] virtual std::shared_ptr<abstract_type_resolver> acquire_joined_object_resolver(const std::type_index &root_type, const std::type_index &element_type, const std::string &collection_name) const = 0;
  virtual void register_joined_object_resolver(std::shared_ptr<abstract_type_resolver> &&resolver, const std::type_index& root_type, const std::string& join_column) = 0;
};

class joined_object_resolver_factory : public abstract_joined_object_resolver_factory {
public:
  template<class Type>
  [[nodiscard]] std::shared_ptr<object_resolver<Type>> resolver(const std::type_index &root_type, const std::string &collection_name) const {
    const auto res = acquire_joined_object_resolver(root_type, typeid(Type), collection_name);
    if (!res) {
      return std::dynamic_pointer_cast<object_resolver<Type>>(res);
    }

    return std::dynamic_pointer_cast<object_resolver<Type>>(res);
  }
};
}
#endif //MATADOR_CONTAINER_RESOLVER_FACTORY_HPP