//
// Created by sascha on 1/29/16.
//

#ifndef OOS_SERIALIZABLE_HPP
#define OOS_SERIALIZABLE_HPP

#include "tools/basic_identifier.hpp"
#include "tools/identifiable_holder.hpp"
#include "tools/varchar.hpp"
#include "tools/cascade_type.hpp"

#include <string>
#include <memory>

namespace oos {

class time;
class date;

class serializer
{
public:
  virtual ~serializer() {}

  virtual void serialize(const char*, char&) = 0;
  virtual void serialize(const char*, short&) = 0;
  virtual void serialize(const char*, int&) = 0;
  virtual void serialize(const char*, long&) = 0;
  virtual void serialize(const char*, unsigned char&) = 0;
  virtual void serialize(const char*, unsigned short&) = 0;
  virtual void serialize(const char*, unsigned int&) = 0;
  virtual void serialize(const char*, unsigned long&) = 0;
  virtual void serialize(const char*, bool&) = 0;
  virtual void serialize(const char*, float&) = 0;
  virtual void serialize(const char*, double&) = 0;
  virtual void serialize(const char*, char *, size_t) = 0;
  virtual void serialize(const char*, std::string&) = 0;
  virtual void serialize(const char*, oos::time&) = 0;
  virtual void serialize(const char*, oos::date&) = 0;
  virtual void serialize(const char*, oos::basic_identifier &x) = 0;
  virtual void serialize(const char*, oos::identifiable_holder &x, cascade_type) = 0;

  template < unsigned int C >
  void serialize(const char *id, oos::varchar<C> &x)
  {
    std::string val = x.str();
    serialize(id, val);
  }

  template < class HAS_MANY >
  void serialize(const char*, HAS_MANY&, const char*, const char*) {}
};

}
#endif //OOS_SERIALIZABLE_HPP
