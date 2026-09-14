#include "matador/query/resolver/resolver_producer.hpp"

namespace matador::query {
const std::type_index &object_resolver_producer::type() const {
  return type_;
}

object_resolver_producer::object_resolver_producer(const std::type_index &type) : type_(type) {}

const std::type_index &joined_object_resolver_producer::root_type() const {
  return root_type_;
}

const std::type_index &joined_object_resolver_producer::type() const {
  return type_;
}

const std::string &joined_object_resolver_producer::collection_name() const {
  return collection_name_;
}

joined_object_resolver_producer::joined_object_resolver_producer(const std::type_index &root_type,
                                                                 const std::type_index &type,
                                                                 std::string collection_name)
: root_type_(root_type), type_(type), collection_name_(std::move(collection_name)) {}
} // namespace matador::query