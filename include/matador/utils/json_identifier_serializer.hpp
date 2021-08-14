#ifndef MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
#define MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP

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

namespace matador {

class basic_identifier;

class OOS_UTILS_API json_identifier_serializer : public serializer
{
public:
  json_identifier_serializer() = default;

  std::string serialize(basic_identifier &x);

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
  void serialize(const char*, matador::time&) override {}
  void serialize(const char*, matador::date&) override {}
  void serialize(const char*, matador::basic_identifier &) override {}
  void serialize(const char*, matador::identifiable_holder &, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, cascade_type) override {}

private:
  std::string json_;
};

}

#endif //MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
