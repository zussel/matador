//
// Created by sascha on 4/10/15.
//

#ifndef PRIMARY_KEY_SERIALIZER_HPP
#define PRIMARY_KEY_SERIALIZER_HPP

#include "object/object_atomizer.hpp"
#include "object/serializable.hpp"

#include <stdexcept>

namespace oos {

class serializable;
class object_base_ptr;
class basic_identifier;
class prototype_node;

/**
 * @class identifier_resolver
 * @brief Returns the primary key of an object
 *
 * Returns the primary key class of a serializable
 * object. If object doesn't have a primary key
 * nullptr is returned
 */
class identifier_resolver : public generic_object_writer<identifier_resolver>
{
public:
  identifier_resolver();
  virtual ~identifier_resolver();

  template < class T >
  static basic_identifier* resolve(typename std::enable_if<!std::is_same<T, oos::serializable>::value>::type* = 0)
  {
    T obj;

    return resolve(&obj);
  }

  template < class T >
  static basic_identifier* resolve(typename std::enable_if<std::is_same<T, oos::serializable>::value>::type* = 0)
  {
    return nullptr;
  }

  static basic_identifier* resolve(serializable *obj)
  {
    identifier_resolver resolver;
    obj->serialize(resolver);
    if (!resolver.id_) {
      return nullptr;
    }
    return resolver.id_;
  }

  template < class T >
  void write_value(const char*, const T&) {}

  void write_value(const char*, const char*, int) {}
  void write_value(const char *id, const basic_identifier &x);

private:
  basic_identifier *id_ = nullptr;
};

}

#endif /* PRIMARY_KEY_SERIALIZER_HPP */
