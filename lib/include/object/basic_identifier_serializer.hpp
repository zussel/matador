/**
 * @author sascha on 4/19/16.
 */

#ifndef OOS_BASIC_IDENTIFIER_SERIALIZER_HPP
#define OOS_BASIC_IDENTIFIER_SERIALIZER_HPP

#include "tools/serializer.hpp"

namespace oos {

class byte_buffer;

class basic_identifier_serializer : public serializer {
public:
  explicit basic_identifier_serializer(byte_buffer &buffer);

  void serialize(basic_identifier &x);
  void deserialize(basic_identifier &x);

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
  byte_buffer &buffer_;
  bool restore_ = false;
};

}
#endif //OOS_BASIC_IDENTIFIER_SERIALIZER_HPP
