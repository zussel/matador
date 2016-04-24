/**
 * @author sascha on 4/19/16.
 */

#ifndef OOS_BASIC_IDENTIFIER_SERIALIZER_HPP
#define OOS_BASIC_IDENTIFIER_SERIALIZER_HPP

#include <typeindex>
#include <iostream>
#include "tools/serializer.hpp"
#include "tools/byte_buffer.hpp"

namespace oos {

class byte_buffer;

class basic_identifier_serializer : public serializer {
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
      std::cout << "restore type " << typeid(T).name() << " (size: " << sizeof(x);
      buffer_->release(&x, sizeof(x));
      std::cout << ", value: " << x << ")\n" << std::flush;
    } else {
      std::cout << "backup type " << typeid(T).name() << " (value: " << x << ", size: " << sizeof(x) << ")\n" << std::flush;
      buffer_->append(&x, sizeof(x));
    }
  }

private:
  byte_buffer *buffer_ = nullptr;
  std::unique_ptr<basic_identifier> basic_identifier_;
  bool restore_ = false;
};

}
#endif //OOS_BASIC_IDENTIFIER_SERIALIZER_HPP
