#include "matador/query/relation_endpoint.hpp"

namespace matador::query {
relation_endpoint::relation_endpoint(std::string field_name, const relation_type type, const schema_node &node)
: field_name_(std::move(field_name))
, type_(type)
, node_(&node) {
}

std::string relation_endpoint::field_name() const {
  return field_name_;
}

relation_type relation_endpoint::type() const {
  return type_;
}

std::string relation_endpoint::type_name() const {
  return relation_type_enum.to_string(type_).value();
}

const schema_node &relation_endpoint::node() const {
  return *node_;
}

bool relation_endpoint::is_has_one() const {
  return type_ == relation_type::HasOne;
}

bool relation_endpoint::is_has_many() const {
  return type_ == relation_type::HasMany;
}

bool relation_endpoint::is_belongs_to() const {
  return type_ == relation_type::BelongsTo;
}

std::shared_ptr<relation_endpoint> relation_endpoint::foreign_endpoint() const {
  return foreign_endpoint_.lock();
}

void relation_endpoint::link_foreign_endpoint( const std::shared_ptr<relation_endpoint>& endpoint ) {
  foreign_endpoint_ = endpoint;
}
}