#include "matador/query/builder/builder.hpp"

namespace matador::query {

column_builder::column_builder(std::string column_name, const utils::basic_type type, const size_t size)
: column_name_(std::move(column_name))
, type_(type)
, size_(size) {}

column_builder::operator class query::column() const {
  return column::make_plain(column_name_, "", type_, {size_, constraints_});
}

column_builder& column_builder::not_null() {
  constraints_ |= column_constraint::NotNull;
  return *this;
}

column_builder & column_builder::primary_key() {
  constraints_ |= column_constraint::PrimaryKey;
  return *this;
}

column_builder & column_builder::unique() {
  constraints_ |= column_constraint::Unique;
  return *this;
}

column_builder& column_builder::identity() {
  constraints_ |= column_constraint::Identity;
  return *this;
}

constraint_builder& constraint_builder::constraint( std::string name ) {
  constraint_name_ = std::move(name);
  return *this;
}

constraint_builder& constraint_builder::primary_key( std::string name ) {
  column_name_ = std::move(name);
  type_ = column_constraint::PrimaryKey;
  return *this;
}

constraint_builder& constraint_builder::foreign_key( std::string name ) {
  column_name_ = std::move(name);
  type_ = column_constraint::ForeignKey;
  return *this;
}

constraint_builder& constraint_builder::references( std::string table, std::string column ) {
  this->ref_table_name_ = std::move(table);
  this->ref_column_name_ = std::move(column);
  return *this;
}

constraint_builder::operator class constraint() const {
  if (ref_table_name_.empty()) {
    return constraint::make_column_constraint(constraint_name_, column_name_, type_);
  }
  return constraint::make_fk_constraint(constraint_name_, column_name_, ref_table_name_, ref_column_name_);
}

constraint_builder constraint( std::string name ) {
  constraint_builder builder;
  return builder.constraint(std::move(name));
}

column_builder col(std::string name, const utils::basic_type type, const size_t size) {
  return column_builder(std::move(name), type, size);
}

}