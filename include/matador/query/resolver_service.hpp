#ifndef MATADOR_RESOLVER_SERVICE_HPP
#define MATADOR_RESOLVER_SERVICE_HPP

#include "matador/query/object_resolver.hpp"
#include "matador/query/collection_resolver.hpp"
#include "matador/query/resolver/producer_object_resolver_factory.hpp"

namespace matador::query {
class resolver_service final {
public:
  template<class Type>
  std::shared_ptr<class object_resolver<Type>> resolver() const {
    return object_resolver_factory_.resolver<Type>();
  }

  template<class Type>
  std::shared_ptr<collection_resolver<Type>> joined_collection_resolver(const std::type_index &root_type, const std::string &collection_name) const {
    return joined_collection_resolver_factory_.resolver<Type>(root_type, collection_name);
  }

  template<class Type>
  std::shared_ptr<object_resolver<Type>> joined_object_resolver(const std::type_index &root_type, const std::string &join_column) const {
    return joined_object_resolver_factory_.resolver<Type>(root_type, join_column);
  }

  void register_object_resolver(std::shared_ptr<abstract_type_resolver> &&resolver);
  void register_collection_resolver(std::shared_ptr<abstract_joined_resolver>&& resolver);
  void register_joined_object_resolver(std::shared_ptr<abstract_type_resolver>&& resolver, const std::type_index& root_type, const std::string& join_column);

private:
  producer_object_resolver_factory object_resolver_factory_;
  producer_joined_collection_resolver_factory joined_collection_resolver_factory_;
  producer_joined_object_resolver_factory joined_object_resolver_factory_;

};
}

#endif //MATADOR_RESOLVER_SERVICE_HPP
