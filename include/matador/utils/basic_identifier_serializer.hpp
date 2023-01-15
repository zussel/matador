#ifndef OOS_BASIC_IDENTIFIER_SERIALIZER_HPP
#define OOS_BASIC_IDENTIFIER_SERIALIZER_HPP

#include "matador/utils/export.hpp"

#include "matador/utils/serializer.hpp"
#include "matador/utils/byte_buffer.hpp"

#include <typeindex>
#include <iostream>

namespace matador {

class byte_buffer;

/// @cond MATADOR_DEV

class OOS_UTILS_API basic_identifier_serializer : public serializer {
public:
  basic_identifier_serializer() = default;

  void serialize(basic_identifier &x, byte_buffer &buffer);
  void deserialize(basic_identifier &x, byte_buffer &buffer);

  void on_attribute(const char*, char&) override;
  void on_attribute(const char*, short&) override;
  void on_attribute(const char*, int&) override;
  void on_attribute(const char*, long&) override;
  void on_attribute(const char*, long long&) override;
  void on_attribute(const char*, unsigned char&) override;
  void on_attribute(const char*, unsigned short&) override;
  void on_attribute(const char*, unsigned int&) override;
  void on_attribute(const char*, unsigned long&) override;
  void on_attribute(const char*, unsigned long long&) override;
  void on_attribute(const char*, bool&) override;
  void on_attribute(const char*, float&) override;
  void on_attribute(const char*, double&) override;
  void on_attribute(const char*, char *, size_t) override;
  void on_attribute(const char*, std::string&) override;
  void on_attribute(const char*, std::string&, size_t) override;
  void on_attribute(const char*, matador::time&) override;
  void on_attribute(const char*, matador::date&) override;
  void on_primary_key(const char*, matador::basic_identifier &) override {}
  void on_belongs_to(const char*, matador::identifiable_holder &, cascade_type) override {}
  void on_has_one(const char*, matador::identifiable_holder &, cascade_type) override {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) override {}

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
