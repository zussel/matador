#ifndef OOS_SERIALIZABLE_HPP
#define OOS_SERIALIZABLE_HPP

#include "matador/utils/export.hpp"

#include "matador/utils/cascade_type.hpp"
#include "matador/utils/access.hpp"

#include <string>
#include <memory>

namespace matador {

class time;
class date;
class identifiable_holder;
class abstract_container;
class field_attributes;
class foreign_attributes;

/**
 * @brief A base class to serialize objects
 *
 * This class can be used as base class to create
 * a concrete serializer class to serialize objects.
 *
 * It is used if one need to pass a base class of a
 * serializer instead a template.
 */
class OOS_UTILS_API serializer
{
public:
  virtual ~serializer() = default;

  /**
   * @brief Interface to serialize a char with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, char &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a short with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, short &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a int with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, int &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a long with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, long &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a long long with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, long long &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a unsigned char with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, unsigned char &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a unsigned short with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, unsigned short &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a unsigned int with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, unsigned int &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a unsigned long long with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, unsigned long long &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a unsigned long with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, unsigned long &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a bool with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, bool &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a float with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, float &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a double with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, double &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a char string with given id
   *
   * @param id The id of the value
   * @param x The string value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, char *x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a string with given id
   *
   * @param id The id of the value
   * @param x The string to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, std::string &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a time with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, matador::time &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize a date with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr The field attributes
   */
  virtual void on_attribute(const char *id, matador::date &x, const field_attributes &attr) = 0;
  /**
   * @brief Interface to serialize an object with given id and cascade type
   *
   * @param id The id of the value
   * @param x The object to be serialized
   * @param cascade The cascade type
   */
  virtual void on_belongs_to(const char *id, matador::identifiable_holder &x, const foreign_attributes &attr) = 0;
  /**
   * @brief Interface to serialize an object with given id and cascade type
   *
   * @param id The id of the value
   * @param x The object to be serialized
   * @param cascade The cascade type
   */
  virtual void on_has_one(const char *id, matador::identifiable_holder &x, const foreign_attributes &attr) = 0;

  /**
   * @fn void on_has_many_to_many(abstract_container &x, const char *join_column, const foreign_attributes &attr)
   * @brief Interface to serialize a relation with given id
   *
   * @param x The value to be serialized
   * @param join_column The name of the join column
   * @param attr Foreign attributes
   */
  virtual void on_has_many(abstract_container &, const char *, const foreign_attributes &attr) = 0;

  /**
   * @fn void on_has_many_to_many(const char *id, abstract_container &x, const char *join_column, const char *inverse_join_column, const foreign_attributes &attr)
   * @brief Interface to serialize a relation with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param join_column The name of the join column name
   * @param inverse_join_column The name of the inverse join column name
   * @param attr Foreign attributes
   */
  virtual void on_has_many_to_many(const char *, abstract_container &, const char *, const char *, const foreign_attributes &attr) = 0;

  /**
   * @fn void on_has_many_to_many(const char *id, abstract_container &x, const foreign_attributes &attr)
   * @brief Interface to serialize a relation with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   * @param attr Foreign attributes
   */
  virtual void on_has_many_to_many(const char *, abstract_container &, const foreign_attributes &attr) = 0;
};

}
#endif //OOS_SERIALIZABLE_HPP
