#include "matador/query/resolver/abstract_joined_resolver.hpp"

namespace matador::query {
const std::type_index& abstract_joined_resolver::root_type() const {
  return root_type_;
}

const std::string& abstract_joined_resolver::collection_name() const {
  return collection_name_;
}

abstract_joined_resolver::abstract_joined_resolver(const std::type_index& root_type, const std::type_index& type, std::string collection_name)
: abstract_type_resolver(type)
, root_type_(root_type)
, collection_name_(std::move(collection_name)) {}
}