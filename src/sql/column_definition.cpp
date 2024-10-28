#include "matador/sql/column_definition.hpp"

#include <ostream>
#include <utility>

namespace matador::sql {

column_definition::column_definition(const char *name)
  : name_(name)
  , attributes_(utils::null_attributes)
{}

column_definition::column_definition(std::string name)
  : name_(std::move(name))
  , attributes_(utils::null_attributes)
{}

column_definition::column_definition(std::string name, const data_type type, const utils::field_attributes& attr, const null_option null_opt, const size_t index)
  : name_(std::move(name))
  , index_(index)
  , attributes_(attr)
  , null_option_(null_opt)
  , type_(type)
{}

column_definition::column_definition(std::string name, const data_type type, const size_t index, std::string ref_table, std::string ref_column,
                                     const utils::field_attributes& attr, const null_option null_opt)
  : name_(std::move(name))
  , index_(index)
  , attributes_(attr)
  , null_option_(null_opt)
  , type_(type)
  , ref_table_(std::move(ref_table))
  , ref_column_(std::move(ref_column))
{}

const std::string &column_definition::name() const
{
  return name_;
}

int column_definition::index() const
{
  return index_;
}

const utils::field_attributes &column_definition::attributes() const
{
  return attributes_;
}

bool column_definition::is_nullable() const
{
  return null_option_ == null_option::NULLABLE;
}

data_type column_definition::type() const
{
  return type_;
}

const std::string &column_definition::ref_table() const
{
  return ref_table_;
}

const std::string &column_definition::ref_column() const
{
  return ref_column_;
}

bool column_definition::is_foreign_reference() const
{
  return !ref_column_.empty() && !ref_table_.empty();
}

bool column_definition::is_integer() const
{
  return type_ >= data_type::type_char && type_ <= data_type::type_unsigned_long_long;
}

bool column_definition::is_floating_point() const
{
  return type_ == data_type::type_float || type_ == data_type::type_double;
}

bool column_definition::is_bool() const
{
  return type_ == data_type::type_bool;
}

bool column_definition::is_string() const
{
  return type_ == data_type::type_text;
}

bool column_definition::is_varchar() const
{
  return type_ == data_type::type_varchar;
}

bool column_definition::is_date() const
{
  return type_ == data_type::type_date;
}

bool column_definition::is_time() const
{
  return type_ == data_type::type_time;
}

bool column_definition::is_blob() const
{
  return type_ == data_type::type_blob;
}

bool column_definition::is_null() const
{
  return type_ == data_type::type_null;
}

bool column_definition::is_unknown() const
{
  return type_ == data_type::type_unknown;
}

void column_definition::type(data_type type)
{
  type_ = type;
}

std::string column_definition::str() const
{
  if (std::holds_alternative<std::string>(value_)) {
    return std::get<std::string>(value_);
  }

  any_type_to_visitor<std::string> visitor;
  std::visit(visitor, const_cast<utils::any_type &>(value_));
  return visitor.result;
}

std::ostream& operator<<(std::ostream &out, const column_definition &col)
{
  out << col.str();
  return out;
}

column_definition make_column(const std::string &name, data_type type, utils::field_attributes attr, null_option null_opt)
{
  return {name, type, attr, null_opt};
}

template<>
column_definition make_column<std::string>(const std::string &name, utils::field_attributes attr, null_option null_opt)
{
  return make_column(name, object::data_type_traits<std::string>::type(attr.size()), attr, null_opt);
}

template<>
column_definition make_pk_column<std::string>(const std::string &name, size_t size)
{
  return make_column<std::string>(name, {size, utils::constraints::FOREIGN_KEY});
}

template<>
[[maybe_unused]] column_definition make_fk_column<std::string>(const std::string &name, size_t size, const std::string &ref_table,
                                                               const std::string &ref_column)
{
  return {name, object::data_type_traits<std::string>::type(size), 0, ref_table, ref_column, {size, utils::constraints::FOREIGN_KEY}, null_option::NOT_NULL};
}
}
