#include "matador/query/resolver_service.hpp"

namespace matador::query {
void resolver_service::register_object_resolver(std::shared_ptr<abstract_type_resolver>&& resolver) {
  object_resolver_factory_.register_object_resolver(std::move(resolver));
}

void resolver_service::register_collection_resolver(std::shared_ptr<abstract_joined_resolver>&& resolver) {
  joined_collection_resolver_factory_.register_collection_resolver(std::move(resolver));
}

void resolver_service::register_joined_object_resolver(std::shared_ptr<abstract_type_resolver>&& resolver, const std::type_index& root_type, const std::string& join_column) {
  joined_object_resolver_factory_.register_joined_object_resolver(std::move(resolver), root_type, join_column);
}
}