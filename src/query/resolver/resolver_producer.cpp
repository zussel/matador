#include "matador/query/resolver/resolver_producer.hpp"

namespace matador::query {
const std::type_index &abstract_object_resolver_producer::type() const {
  return type_;
}

abstract_object_resolver_producer::abstract_object_resolver_producer(const std::type_index &type)
: type_(type) {}

const std::type_index &abstract_joined_object_resolver_producer::root_type() const {
  return root_type_;
}

const std::type_index &abstract_joined_object_resolver_producer::type() const {
  return type_;
}

const std::string &abstract_joined_object_resolver_producer::collection_name() const {
  return collection_name_;
}

abstract_joined_object_resolver_producer::abstract_joined_object_resolver_producer(const std::type_index &root_type,
                                                                 const std::type_index &type,
                                                                 std::string collection_name)
: root_type_(root_type)
, type_(type)
, collection_name_(std::move(collection_name)) {}
}