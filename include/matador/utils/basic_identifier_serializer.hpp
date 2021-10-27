#ifndef OOS_BASIC_IDENTIFIER_SERIALIZER_HPP
#define OOS_BASIC_IDENTIFIER_SERIALIZER_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_UTILS_API
#endif

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

  void serialize(const char*, char&) override;
  void serialize(const char*, short&) override;
  void serialize(const char*, int&) override;
  void serialize(const char*, long&) override;
  void serialize(const char*, long long&) override;
  void serialize(const char*, unsigned char&) override;
  void serialize(const char*, unsigned short&) override;
  void serialize(const char*, unsigned int&) override;
  void serialize(const char*, unsigned long&) override;
  void serialize(const char*, unsigned long long&) override;
  void serialize(const char*, bool&) override;
  void serialize(const char*, float&) override;
  void serialize(const char*, double&) override;
  void serialize(const char*, char *, size_t) override;
  void serialize(const char*, std::string&) override;
  void serialize(const char*, std::string&, size_t) override;
  void serialize(const char*, matador::time&) override;
  void serialize(const char*, matador::date&) override;
  void serialize(const char*, matador::basic_identifier &x) override;
  void serialize(const char*, matador::identifiable_holder &x, cascade_type) override;
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, cascade_type) override {}

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
