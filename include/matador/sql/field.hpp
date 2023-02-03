#ifndef OOS_FIELD_HPP
#define OOS_FIELD_HPP

//#include "matador/sql/export.hpp"

#include "matador/sql/types.hpp"

#include <string>

namespace matador {

/**
 * @brief Describes a database column field
 */
//class OOS_SQL_API field
class field
{
public:
  /**
   * @brief Creates an empty field
   */
  field();

  /**
   * @brief Creates a named field
   * @param name Name of the column field
   */
  explicit field(const char *name);

  /**
   * @brief Creates a named field
   * @param name Name of the column field
   */
  explicit field(const std::string &name);

  /**
   * @brief Destroy a field
   */
  ~field();

  /**
   * @brief Returns the index of the field in table
   * @return Index of the field in table
   */
  size_t index() const;

  /**
   * @brief Sets the index in table of the field
   * @param i The index of the field
   */
  void index(size_t i);

  /**
   * @brief Returns the name of the field
   * @return Name of the field
   */
  std::string name() const;

  /**
   * @brief Sets the name of the field
   * @param n Name of the field to set
   */
  void name(const std::string &n);

  /**
   * @brief Returns the type of the field
   * @return Type of the field
   */
  database_type type() const;

  /**
   * @brief Sets the type of the field
   * @param t Type of the field to set
   */
  void type(database_type t);

  /**
   * @brief Returns the size of the field
   *
   * Return the size of the field if
   * field is a numeric or varchar type.
   *
   * @return Size of the field
   */
  std::size_t size() const;

  /**
   * @brief Sets the size of the field
   *
   * Has only an effect if the type of
   * the field is numeric or varchar.
   *
   * @param s Size of the field to set
   */
  void size(std::size_t s);

  /**
   * @brief Returns the precision of the field
   *
   * Return the precision of the field if
   * field is of type float or double
   *
   * @return Precision of the field
   */
  std::size_t precision() const;

  /**
   * @brief Sets the precision of the field
   *
   * Has only an effect if the field type
   * is float or double.
   *
   * @param p Precision of the field to set
   */
  void precision(std::size_t p);

  /**
   * @brief Returns true if NULL value is not allowed
   * @return True if NULL value is not allowed
   */
  bool is_not_null() const;

  /**
   * @brief Set to true if NULL value is not allowed for this field
   * @param nn True if NULL value is not allowed
   */
  void not_null(bool nn);

  /**
   * @brief Returns the default value as string
   * @return Default value as string
   */
  std::string default_value() const;

  /**
   * @brief Sets the default value as string
   * @param value Default value as string
   */
  void default_value(const std::string &value);

private:
  size_t index_ = 0;
  std::string name_;
  database_type type_ = database_type::type_unknown;
  std::size_t size_ = 0;
  std::size_t precision_ = 0;
  bool not_null_ = false;
  std::string default_value_;
};

}
#endif //OOS_FIELD_HPP
