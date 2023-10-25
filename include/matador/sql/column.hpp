#ifndef OOS_COLUMN_HPP
#define OOS_COLUMN_HPP

#include <utility>

#include "matador/sql/token.hpp"
#include "matador/sql/value.hpp"
#include "matador/sql/basic_dialect.hpp"

namespace matador {

enum class t_build_options {
  empty = 0 << 0,
  with_quotes = 1 << 0,
  with_type = 1 << 1,
  with_value = 1 << 2
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
 * @brief Represents the base class of a database column.
 *
 * Represents a database column consisting of
 * name.
 */
class column : public detail::token
{
public:
  enum class t_use_case {
    COLUMN_NAME, CREATE_COLUMN, UPDATE_COLUMN
  };
  enum class t_quotes_options {
    WITH_QUOTES, WITHOUT_QUOTES
  };

  explicit column(std::string name, t_quotes_options quotes_options = t_quotes_options::WITHOUT_QUOTES);
  column(std::string name, data_type type, size_t index, const field_attributes &attr);
  column(std::string name, const any &val);

  column(const column &x) = default;
  column& operator=(const column &x) = default;

  column(column &&x) = default;
  column& operator=(column &&x) = default;

  /**
   * @brief Interface according to the visitor pattern.
   *
   * @param visitor The visitor object to be accepted
   */
  void accept(token_visitor &visitor) override;

  const std::string& name() const;
  void name(const std::string& n);

  bool with_quotes() const;
  bool without_quotes() const;

  bool is_name_column() const;
  bool is_create_column() const;
  bool is_update_column() const;

  data_type type() const;
  const field_attributes& attributes() const;
  size_t index() const;

  const struct value& value() const;
  struct value& value();
  void value(const struct value &val);

private:
  std::string name_;         /**< Name of the column */
  t_use_case use_case_{t_use_case::COLUMN_NAME};
  t_quotes_options quotes_options_{t_quotes_options::WITHOUT_QUOTES};

  data_type type_{data_type::type_unknown};
  field_attributes attributes_;
  std::size_t index_{};

  struct value val_;
};

/**
 * User defined literal to have a shortcut creating a column object
 * @param name Name of the column
 * @param len Length of the column name
 * @return A column object with given name
 */
column operator "" _col(const char *name, size_t len);

/**
 * Creates a column for update statement
 *
 * @param name
 * @param val
 * @return
 */
column make_column(const std::string &name, const matador::any& val = {});

/**
 * Creates a column for create statement
 *
 * @param name
 * @param type
 * @param index
 * @param attr
 * @return
 */
column make_column(const std::string &name, data_type type, size_t index, const field_attributes &attr = null_attributes);

template < typename Type >
column make_column(const std::string &name, const field_attributes &attr = null_attributes) {
  return make_column(name, data_type_traits<Type>::builtin_type(attr.size()), 0, attr);
}

template < typename Type >
column make_column(const std::string &name, const Type &val, const field_attributes &/*attr*/ = null_attributes) {
  return {name, val};
}

/**
 * Create primary key column for create statement
 *
 * @tparam Type
 * @param name
 * @param max_size
 * @return
 */
template < typename Type >
column make_pk_column(const std::string &name, size_t max_size = 0) {
  return {name, data_type_traits<Type>::builtin_type(max_size), max_size, field_attributes{ max_size, constraints::PRIMARY_KEY | constraints::NOT_NULL}};
}

template <>
column make_pk_column<std::string>(const std::string &name, size_t max_size);

column make_pk_column(const std::string &name, data_type type, size_t index, size_t max_size = 0);

//std::unique_ptr<column> make_null_column(const std::string &name, const field_attributes &attr);

/// @endcond

}
#endif //OOS_COLUMN_HPP
