/**
 * @author sascha on 4/19/16.
 */

#ifndef OOS_BASIC_IDENTIFIER_SERIALIZER_HPP
#define OOS_BASIC_IDENTIFIER_SERIALIZER_HPP

#ifdef _MSC_VER
#ifdef oos_utils_EXPORTS
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

#include "oos/utils/serializer.hpp"
#include "oos/utils/byte_buffer.hpp"

#include <typeindex>
#include <iostream>

namespace oos {

class byte_buffer;

/// @cond OOS_DEV

class OOS_API basic_identifier_serializer : public serializer {
public:
  basic_identifier_serializer();

  void serialize(basic_identifier &x, byte_buffer &buffer);
  void deserialize(basic_identifier &x, byte_buffer &buffer);

  virtual void serialize(const char*, char&);
  virtual void serialize(const char*, short&);
  virtual void serialize(const char*, int&);
  virtual void serialize(const char*, long&);
  virtual void serialize(const char*, unsigned char&);
  virtual void serialize(const char*, unsigned short&);
  virtual void serialize(const char*, unsigned int&);
  virtual void serialize(const char*, unsigned long&);
  virtual void serialize(const char*, bool&);
  virtual void serialize(const char*, float&);
  virtual void serialize(const char*, double&);
  virtual void serialize(const char*, char *, size_t);
  virtual void serialize(const char*, std::string&);
  virtual void serialize(const char*, oos::varchar_base&);
  virtual void serialize(const char*, oos::time&);
  virtual void serialize(const char*, oos::date&);
  virtual void serialize(const char*, oos::basic_identifier &x);
  virtual void serialize(const char*, oos::identifiable_holder &x, cascade_type);

private:
  template < class T >
  void serialize_value(T &x)
  {
    if (restore_) {
      buffer_->release(&x, sizeof(x));
    } else {
      buffer_->append(&x, sizeof(x));
    }
  }

private:
  byte_buffer *buffer_ = nullptr;
  std::unique_ptr<basic_identifier> basic_identifier_;
  bool restore_ = false;
};

/// @endcond

}
#endif //OOS_BASIC_IDENTIFIER_SERIALIZER_HPP
