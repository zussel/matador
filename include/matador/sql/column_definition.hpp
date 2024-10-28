#ifndef QUERY_COLUMN_DEFINITION_HPP
#define QUERY_COLUMN_DEFINITION_HPP

#include "matador/sql/any_type_to_visitor.hpp"

#include "matador/object/data_type_traits.hpp"

#include "matador/utils/data_types.hpp"
#include "matador/utils/field_attributes.hpp"

#include <optional>
#include <vector>

namespace matador::sql {

enum class null_option : uint8_t {
  NULLABLE, NOT_NULL
};

class column_definition {
public:
  explicit column_definition(const char *name); // NOLINT(*-explicit-constructor)
  explicit column_definition(std::string name); // NOLINT(*-explicit-constructor)

  column_definition(const column_definition&) = default;
  column_definition& operator=(const column_definition&) = default;
  column_definition(column_definition&&) noexcept = default;
  column_definition& operator=(column_definition&&) noexcept = default;

  template<typename Type>
  explicit column_definition(std::string name, const utils::field_attributes& attr)
  : column_definition(std::move(name), object::data_type_traits<Type>::type(attr.size()), attr)
  {}

  template<typename Type>
  column_definition(std::string name, const Type &, const utils::field_attributes& attr, null_option null_opt)
  : column_definition(std::move(name), object::data_type_traits<Type>::type(attr.size()), attr, null_opt)
  {}

  template<size_t SIZE>
  column_definition(std::string name, const char (&)[SIZE], const utils::field_attributes& attr, const null_option null_opt)
  : column_definition(std::move(name), object::data_type_traits<const char*>::type(attr.size()), attr, null_opt)
  {}

  column_definition(std::string name, data_type type, const utils::field_attributes&, null_option null_opt, size_t index = 0);

  template<typename Type>
  column_definition(std::string name, std::string ref_table, std::string ref_column, const utils::field_attributes& attr, null_option null_opt)
  : column_definition(std::move(name), object::data_type_traits<Type>::type(attr.size()), ref_table, ref_column, attr, null_opt)
  {}

  column_definition(std::string name, data_type type, size_t index, std::string ref_table, std::string ref_column, const utils::field_attributes& attr, null_option null_opt);

  [[nodiscard]] const std::string& name() const;
  [[nodiscard]] int index() const;
  [[nodiscard]] const utils::field_attributes& attributes() const;
  [[nodiscard]] bool is_nullable() const;
  [[nodiscard]] data_type type() const;
  [[nodiscard]] const std::string& ref_table() const;
  [[nodiscard]] const std::string& ref_column() const;
  [[nodiscard]] bool is_foreign_reference() const;

  [[nodiscard]] bool is_integer() const;
  [[nodiscard]] bool is_floating_point() const;
  [[nodiscard]] bool is_bool() const;
  [[nodiscard]] bool is_string() const;
  [[nodiscard]] bool is_varchar() const;
  [[nodiscard]] bool is_date() const;
  [[nodiscard]] bool is_time() const;
  [[nodiscard]] bool is_blob() const;
  [[nodiscard]] bool is_null() const;
  [[nodiscard]] bool is_unknown() const;

  void type(data_type type);

  template< typename Type >
  [[nodiscard]] bool is_type_of() const {
    return std::holds_alternative<Type>(value_);
  }

  [[nodiscard]] std::string str() const;

  template<typename Type>
  void set(const Type &value, const utils::field_attributes &attr = utils::null_attributes)
  {
    type_ = object::data_type_traits<Type>::type(attr.size());
    attributes_ = attr;
    value_ = value;
  }

  void set(const std::string &value, const utils::field_attributes &attr)
  {
    type_ = object::data_type_traits<std::string>::type(attr.size());
    attributes_ = attr;
    value_ = value;
  }

  void set(const char *value, const utils::field_attributes &attr)
  {
    type_ = object::data_type_traits<std::string>::type(attr.size());
    attributes_ = attr;
    value_ = value;
  }

  template<class Type>
  Type as() const
  {
    const Type* ptr= std::get_if<Type>(&value_);
    if (ptr) {
      return *ptr;
    }
    any_type_to_visitor<Type> visitor;
    std::visit(visitor, const_cast<utils::any_type&>(value_));
    return visitor.result;
  }

  friend std::ostream& operator<<(std::ostream &out, const column_definition &col);

private:
  template<class Operator>
  void process(Operator &op)
  {
    op.on_attribute(name_.c_str(), value_, type_, attributes_);
  }

  using data_type_index = std::vector<data_type>;

private:
  static const data_type_index data_type_index_;

  std::string name_;
  int index_{-1};
  utils::field_attributes attributes_;
  null_option null_option_{null_option::NOT_NULL};
  data_type type_{data_type::type_unknown};
  utils::any_type value_;
  std::string ref_table_;
  std::string ref_column_;
};

/**
 * User defined literal to have a shortcut creating a column object
 * @param name Name of the column
 * @param len Length of the column name
 * @return A column object with given name
 */
column_definition make_column(const std::string &name, data_type type, utils::field_attributes attr = utils::null_attributes, null_option null_opt = null_option::NOT_NULL);

template < typename Type >
column_definition make_column(const std::string &name, utils::field_attributes attr = utils::null_attributes, null_option null_opt = null_option::NOT_NULL)
{
    return make_column(name, object::data_type_traits<Type>::type(0), attr, null_opt);
}
template <>
column_definition make_column<std::string>(const std::string &name, utils::field_attributes attr, null_option null_opt);

template < typename Type >
column_definition make_pk_column(const std::string &name, size_t size = 0)
{
    return make_column<Type>(name, { size, utils::constraints::PRIMARY_KEY });
}

template <>
column_definition make_pk_column<std::string>(const std::string &name, size_t size);

template < typename Type >
column_definition make_fk_column(const std::string &name, size_t size, const std::string &ref_table, const std::string &ref_column)
{
    return {name, object::data_type_traits<Type>::type(size), ref_table, ref_column, { size, utils::constraints::FOREIGN_KEY }};
}

template < typename Type >
[[maybe_unused]] column_definition make_fk_column(const std::string &name, const std::string &ref_table, const std::string &ref_column)
{
    return {name, object::data_type_traits<Type>::type(0), 0, ref_table, ref_column, { 0, utils::constraints::FOREIGN_KEY }, null_option::NOT_NULL};
}

template <>
column_definition make_fk_column<std::string>(const std::string &name, size_t size, const std::string &ref_table, const std::string &ref_column);

}
#endif //QUERY_COLUMN_DEFINITION_HPP
