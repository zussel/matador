//
// Created by sascha on 1/29/16.
//

#ifndef OOS_SERIALIZABLE_HPP
#define OOS_SERIALIZABLE_HPP

#include "object/identifier.hpp"
#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"

#include <string>

namespace oos {

class time;
class date;

class serializable
{
public:
  virtual ~serializable() {}

  virtual void serialize(const char*, char) = 0;
  virtual void serialize(const char*, short) = 0;
  virtual void serialize(const char*, int) = 0;
  virtual void serialize(const char*, long) = 0;
  virtual void serialize(const char*, unsigned char) = 0;
  virtual void serialize(const char*, unsigned short) = 0;
  virtual void serialize(const char*, unsigned int) = 0;
  virtual void serialize(const char*, unsigned long) = 0;
  virtual void serialize(const char*, float) = 0;
  virtual void serialize(const char*, double) = 0;
  virtual void serialize(const char*, const std::string&) = 0;
  virtual void serialize(const char*, const oos::time&) = 0;
  virtual void serialize(const char*, const oos::date&) = 0;

  template < class T >
  void serialize(const char *id, oos::identifier<T> &x)
  {
    T val = x.value();
    serialize(id, val);
  }

  template < unsigned int C >
  void serialize(const char *id, oos::varchar<C> &x)
  {
    std::string val = x.str();
    serialize(id, val);
  }

  template < class T >
  void serialize(const char *id, oos::has_one<T> &x)
  {
    if (x.has_primary_key()) {
      std::shared_ptr<basic_identifier> bid = x.primary_key();
      bid->serialize(id, *this);
      bid->is_valid();
    }
  }

  template < class HAS_MANY >
  void serialize(const char*, HAS_MANY&, const char*, const char*) {}
};

}
#endif //OOS_SERIALIZABLE_HPP
