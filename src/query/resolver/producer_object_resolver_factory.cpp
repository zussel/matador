#include "matador/query/resolver/producer_object_resolver_factory.hpp"

namespace matador::query {
std::shared_ptr<abstract_type_resolver> producer_object_resolver_factory::acquire_object_resolver(const std::type_index &type) const {
  if (const auto it = resolvers_.find(type); it != resolvers_.end()) {
    return it->second;
  }
  return nullptr;
}

void producer_object_resolver_factory::register_object_resolver(std::shared_ptr<abstract_type_resolver> &&resolver) {
  resolvers_[resolver->type()] = std::move(resolver);
}
std::shared_ptr<abstract_joined_resolver>
producer_joined_collection_resolver_factory::acquire_collection_resolver(const std::type_index& root_type,
                                                                  const std::type_index& element_type,
                                                                  const std::string& collection_name) const {
  const collection_composite_key key{root_type, element_type, collection_name};
  if (const auto it = resolvers_.find(key); it != resolvers_.end()) {
    return it->second;
  }
  return nullptr;
}
void producer_joined_collection_resolver_factory::register_collection_resolver(std::shared_ptr<abstract_joined_resolver>&& resolver) {
  const collection_composite_key key{resolver->root_type(), resolver->type(), resolver->collection_name()};
  resolvers_[key] = std::move(resolver);
}

std::shared_ptr<abstract_type_resolver> producer_joined_object_resolver_factory::acquire_joined_object_resolver(const std::type_index& root_type,
                                                                                                                        const std::type_index& element_type,
                                                                                                                        const std::string& collection_name) const {
  const collection_composite_key key{root_type, element_type, collection_name};
  if (const auto it = resolvers_.find(key); it != resolvers_.end()) {
    return it->second;
  }
  return nullptr;
}

void producer_joined_object_resolver_factory::register_joined_object_resolver(std::shared_ptr<abstract_type_resolver>&& resolver, const std::type_index& root_type, const std::string& join_column) {
  const collection_composite_key key{root_type, resolver->type(), join_column};
  resolvers_[key] = std::move(resolver);
}
}  // namespace matador::sql
