#include "matador/query/basic_table_info.hpp"

#include "matador/query/schema_node.hpp"

#include <algorithm>

namespace matador::query {
basic_table_info::basic_table_info(const schema_node& node, const std::shared_ptr<class table>& t)
: table_(t)
, node_(node) {}

std::type_index basic_table_info::type_index() const {
  return node_.type_index();
}

std::string basic_table_info::name() const {
  return node_.name();
}

std::shared_ptr<class table> basic_table_info::table() const {
  return table_;
}

const std::vector<column>& basic_table_info::columns() const {
  return table_->columns();
}

const std::vector<constraint> basic_table_info::constraints() const {
  return table_->constraints();
}

bool basic_table_info::has_primary_key() const {
  return table_->has_primary_key();
}

// const identifier& basic_table_info::primary_key() const {
  // return table_->primary_key_column();
// }

const column* basic_table_info::primary_key_attribute() const {
  return table_->primary_key_column();
}

void basic_table_info::update_name(const std::string& name) const {
  table_->update_name(name);
}

basic_table_info::endpoint_iterator basic_table_info::register_relation_endpoint(const std::type_index &type, const std::shared_ptr<relation_endpoint> &endpoint) {
  return relation_endpoints_.insert(std::make_pair(type, endpoint)).first;
}

void basic_table_info::unregister_relation_endpoint(const std::type_index &type) {
  relation_endpoints_.erase(type);
}

basic_table_info::const_endpoint_iterator
basic_table_info::find_relation_endpoint(const std::type_index &type) const {
  return relation_endpoints_.find(type);
}

basic_table_info::endpoint_iterator basic_table_info::find_relation_endpoint(const std::type_index &type) {
  return relation_endpoints_.find(type);
}

basic_table_info::const_endpoint_iterator basic_table_info::find_relation_endpoint(const std::string &field) const {
  return std::find_if(relation_endpoints_.begin(), relation_endpoints_.end(), [&field](const t_endpoint_map::value_type &value) {
    return value.second->field_name() == field;
  });
}

basic_table_info::endpoint_iterator basic_table_info::find_relation_endpoint(const std::string &field) {
  return std::find_if(relation_endpoints_.begin(), relation_endpoints_.end(), [&field](const t_endpoint_map::value_type &value) {
    return value.second->field_name() == field;
  });
}

basic_table_info::endpoint_iterator basic_table_info::endpoint_begin() {
  return relation_endpoints_.begin();
}

basic_table_info::const_endpoint_iterator basic_table_info::endpoint_begin() const {
  return relation_endpoints_.begin();
}

basic_table_info::endpoint_iterator basic_table_info::endpoint_end() {
  return relation_endpoints_.end();
}

basic_table_info::const_endpoint_iterator basic_table_info::endpoint_end() const {
  return relation_endpoints_.end();
}

std::size_t basic_table_info::endpoints_size() const {
  return relation_endpoints_.size();
}

bool basic_table_info::endpoints_empty() const {
  return relation_endpoints_.empty();
}
}