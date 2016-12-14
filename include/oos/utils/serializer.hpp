//
// Created by sascha on 1/29/16.
//

#ifndef OOS_SERIALIZABLE_HPP
#define OOS_SERIALIZABLE_HPP

#ifdef _MSC_VER
#ifdef oos_utils_EXPORTS
#define OOS_UTILS_API __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define OOS_UTILS_API __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include "oos/utils/cascade_type.hpp"
#include "oos/utils/varchar.hpp"
#include "access.hpp"

#include <string>
#include <memory>

namespace oos {

class time;
class date;
class varchar_base;
class identifiable_holder;
class basic_identifier;

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
  virtual ~serializer() {}

  /**
   * @brief Serialize an object of type T.
   *
   * @tparam T Type of x
   * @param x instance of x to be serialized
   */
  template < class T >
  void serialize(T &x)
  {
    oos::access::serialize(*this, x);
  }

  /**
   * @brief Interface to serialize a char with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, char &x) = 0;
  /**
   * @brief Interface to serialize a short with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, short &x) = 0;
  /**
   * @brief Interface to serialize a int with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, int &x) = 0;
  /**
   * @brief Interface to serialize a long with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, long &x) = 0;
  /**
   * @brief Interface to serialize a unsigned char with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, unsigned char &x) = 0;
  /**
   * @brief Interface to serialize a unsigned short with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, unsigned short &x) = 0;
  /**
   * @brief Interface to serialize a unsigned int with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, unsigned int &x) = 0;
  /**
   * @brief Interface to serialize a unsigned long with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, unsigned long &x) = 0;
  /**
   * @brief Interface to serialize a bool with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, bool &x) = 0;
  /**
   * @brief Interface to serialize a float with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, float &x) = 0;
  /**
   * @brief Interface to serialize a double with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, double &x) = 0;
  /**
   * @brief Interface to serialize a char string with given id
   *
   * @param id The id of the value
   * @param x The string value to be serialized
   * @param s The size of the string
   */
  virtual void serialize(const char *id, char *x, size_t s) = 0;
  /**
   * @brief Interface to serialize a string with given id
   *
   * @param id The id of the value
   * @param x The string to be serialized
   */
  virtual void serialize(const char *id, std::string &x) = 0;
  /**
   * @brief Interface to serialize a varchar with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, oos::varchar_base &x) = 0;
  /**
   * @brief Interface to serialize a time with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, oos::time &x) = 0;
  /**
   * @brief Interface to serialize a date with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, oos::date &x) = 0;
  /**
   * @brief Interface to serialize a identifier with given id
   *
   * @param id The id of the value
   * @param x The value to be serialized
   */
  virtual void serialize(const char *id, oos::basic_identifier &x) = 0;
  /**
   * @brief Interface to serialize an object with given id and cascade type
   *
   * @param id The id of the value
   * @param x The object to be serialized
   * @param cascade The cascade type
   */
  virtual void serialize(const char *id, oos::identifiable_holder &x, cascade_type cascade) = 0;

  /**
   * @brief Interface to serialize a varchar with given id
   *
   * @tparam S The size of the varchar
   * @param id The id of the value
   * @param x The value to be serialized
   */
  template <unsigned int S >
  void serialize(const char *id, varchar<S> &x)
  {
    serialize(id, serializer::to_varchar_base(x));
  }

  /**
   * @brief Interface to serialize a identifier with given id
   *
   * @param id The id of the value
   * @param x The identifier to be serialized
   */
  template < class T, typename = typename std::enable_if<
    std::is_base_of<basic_identifier, T>::value &&
    !std::is_same<T, basic_identifier>::value
  >::type >
  void serialize(const char *id, T &x)
  {
    serialize(id, serializer::to_basic_identifier(x));
  }


  /**
   * @fn template < class HAS_MANY > void serialize(const char *id, HAS_MANY &x, const char *owner_id, const char *item_id)
   * @brief Interface to serialize a relation with given id
   *
   * @tparam HAS_MANY The type of the relationship object
   * @param id The id of the value
   * @param x The value to be serialized
   * @param owner_id The name of the owner field
   * @param item_id The name of the item field
   */
  template < class HAS_MANY >
  void serialize(const char *, HAS_MANY &, const char *, const char *) {}

private:
  template <unsigned int S >
  static varchar_base& to_varchar_base(varchar<S> &x) { return x; }

  template < class T, typename = typename std::enable_if<
    std::is_base_of<basic_identifier, T>::value &&
    !std::is_same<T, basic_identifier>::value
  >::type >
  static basic_identifier& to_basic_identifier(T &x) { return x; }
};

}
#endif //OOS_SERIALIZABLE_HPP
