#ifndef MATADOR_RESOLVER_SERVICE_HPP
#define MATADOR_RESOLVER_SERVICE_HPP

#include "matador/query/object_resolver.hpp"
#include "matador/query/collection_resolver.hpp"

namespace matador::query {
class resolver_service final {
public:
  template<class Type>
  std::shared_ptr<class object_resolver<Type>> resolver() const {
    return {};
    // return object_resolver_factory_.resolver<Type>();
  }

  template<class Type>
  std::shared_ptr<collection_resolver<Type>> joined_collection_resolver(const std::type_index &root_type, const std::string &collection_name) const {
    return {};
    // return joined_collection_resolver_factory_.resolver<Type>(root_type, collection_name);
  }

  template<class Type>
  std::shared_ptr<object_resolver<Type>> joined_object_resolver(const std::type_index &root_type, const std::string &join_column) const {
    return {};
    // return joined_object_resolver_factory_.resolver<Type>(root_type, join_column);
  }

};
}

#endif //MATADOR_RESOLVER_SERVICE_HPP
