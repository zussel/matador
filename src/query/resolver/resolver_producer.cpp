#include "matador/query/resolver/resolver_producer.hpp"

namespace matador::query {
const std::type_index &abstract_object_resolver_producer::type() const {
  return type_;
}

const std::string &abstract_object_resolver_producer::where_column() const {
  return where_column_;
}

const std::type_index &abstract_object_resolver_producer::root_type() const {
  return root_type_;
}

abstract_object_resolver_producer::abstract_object_resolver_producer(const std::type_index &type, std::string where_column, const std::type_index& root_type)
: type_(type)
, where_column_(std::move(where_column))
, root_type_(root_type) {}
}
