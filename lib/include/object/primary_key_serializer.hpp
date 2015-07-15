//
// Created by sascha on 4/10/15.
//

#ifndef PRIMARY_KEY_SERIALIZER_HPP
#define PRIMARY_KEY_SERIALIZER_HPP

#include "object/object_atomizer.hpp"

namespace oos {

class serializable;
class object_base_ptr;
class primary_key_base;
class prototype_node;

/**
 * @class primary_key_serializer
 * @brief Returns the primary key of an object
 *
 * Returns the primary key class of a serializable
 * object. If object doesn't have a primary key
 * nullptr is returned
 */
class primary_key_serializer : public generic_object_writer<primary_key_serializer>
{
public:
  primary_key_serializer();
  virtual ~primary_key_serializer();

  primary_key_base* serialize(const serializable *o);

  template < class T >
  void write_value(const char*, const T&) {}

  void write_value(const char*, const char*, int) {}
  void write_value(const char *id, const primary_key_base &x);

private:
  primary_key_base *primary_key_;
};

}

#endif /* PRIMARY_KEY_SERIALIZER_HPP */
