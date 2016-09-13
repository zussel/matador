//
// Created by sascha on 2/23/16.
//

#ifndef OOS_FIELD_HPP
#define OOS_FIELD_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_API
#endif

#include "sql/types.hpp"

#include <string>

namespace oos {

/**
 * @brief Describes a database column field
 */
class OOS_API field
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
  field(const char *name);

  /**
   * @brief Creates a named field
   * @param name Name of the column field
   */
  field(const std::string &name);
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
   * @breif Returns the name of the field
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
  data_type_t type() const;

  /**
   * @brief Sets the type of the field
   * @param t Type of the field to set
   */
  void type(data_type_t t);

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
  data_type_t type_;
  bool not_null_ = false;
  std::string default_value_;
};

}
#endif //OOS_FIELD_HPP
