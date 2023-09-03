#ifndef OOS_COLUMN_HPP
#define OOS_COLUMN_HPP

#include "matador/sql/token.hpp"
#include "matador/sql/value.hpp"
#include "matador/sql/basic_dialect.hpp"

namespace matador {

enum class t_build_options {
  empty = 0 << 0,
  with_quotes = 1 << 0,
  with_type = 1 << 1,
  with_size = 1 << 2
};

inline t_build_options operator|(t_build_options a, t_build_options b)
{
  return static_cast<t_build_options>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline t_build_options operator|=(t_build_options &a, t_build_options b)
{
  return static_cast<t_build_options>(reinterpret_cast<unsigned int&>(a) |= static_cast<unsigned int>(b));
}

inline t_build_options operator&(t_build_options a, t_build_options b)
{
  return static_cast<t_build_options>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

inline bool is_build_options_set(t_build_options source, t_build_options needle)
{
  return static_cast<int>(source & needle) > 0;
}

/**
 * @brief Represents a database column.
 *
 * Represents a database column consisting of
 * name.
 */
struct column : public detail::token
{
  /**
   * @brief Creates a new column with given name
   *
   * @param col The name of the column
   * @param attr Field attributes of the column
   */
  explicit column(std::string col, const field_attributes &attr = null_attributes);

  column(std::string name, const matador::any& val, const field_attributes &attr);

  /**
   * @brief Creates a new column with given name
   *
   * @param col The name of the column
   * @param skip_quotes True if the column shouldn't get quotes
   * @param attr Field attributes of the column
   */
  column(std::string col, t_build_options options, const field_attributes &attr = null_attributes);

  template < typename Type >
  column(std::string name, const Type &val, field_attributes attr, typename std::enable_if<!std::is_same<Type, matador::any>::value>::type* = 0)
  : token(COLUMN)
  , name(std::move(name))
  , val(val)
  , type(data_type_traits<Type>::builtin_type())
  , attributes(attr) {}

  /**
   * @brief Interface according to the visitor pattern.
   *
   * @param visitor The visitor object to be accepted
   */
  void accept(token_visitor &visitor) override;

  std::string name;         /**< Name of the column */
  t_build_options build_options{t_build_options::empty};
  value val;
  data_type type{data_type::type_unknown};
  field_attributes attributes;
  std::size_t index{};
//  bool is_host{false};
};

/**
 * User defined literal to have a shortcut creating a column object
 * @param name Name of the column
 * @param len Length of the column name
 * @return A column object with given name
 */
column operator "" _col(const char *name, size_t len);

std::shared_ptr<column> make_column(const std::string &name, const field_attributes &attr = null_attributes, const matador::any& val = {});

std::shared_ptr<column> make_column(const std::string &name, data_type type, size_t index, const field_attributes &attr = null_attributes);

template < typename Type >
std::shared_ptr<column> make_column(const std::string &name, const field_attributes &attr = null_attributes) {
  return make_column(name, Type{}, attr);
}

template < typename Type >
std::shared_ptr<column> make_column(const std::string &name, const Type &val, const field_attributes &attr = null_attributes) {
  return std::make_shared<column>(name, val, attr);
}

std::shared_ptr<column> make_column(const std::string &name, data_type type, const matador::any& val, t_build_options options, size_t index, const field_attributes &attr = null_attributes);

template < typename Type >
std::shared_ptr<column> make_pk_column(const std::string &name, size_t /*max_size*/ = 0) {
  return make_column(name, Type{}, constraints::PRIMARY_KEY);
}

template <>
std::shared_ptr<column> make_pk_column<std::string>(const std::string &name, size_t max_size);

std::shared_ptr<column> make_pk_column(const std::string &name, data_type type, size_t index, size_t max_size = 0);

std::shared_ptr<column> make_null_column(const std::string &name, const field_attributes &attr);

/// @endcond

}
#endif //OOS_COLUMN_HPP
