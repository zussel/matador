#include "matador/query/resolver/joined_collection_resolver_producer.hpp"

namespace matador::query {
const std::type_index& joined_collection_resolver_producer::root_type() const {
  return root_type_;
}

const std::type_index& joined_collection_resolver_producer::type() const {
  return type_;
}

const std::string& joined_collection_resolver_producer::join_column_name() const {
  return join_column_name_;
}

joined_collection_resolver_producer::joined_collection_resolver_producer(const std::type_index& root_type,
                                                           const std::type_index& type,
                                                           std::string join_column_name)
: root_type_(root_type), type_(type), join_column_name_(std::move(join_column_name)) {}
}