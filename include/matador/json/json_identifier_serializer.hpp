#ifndef MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
#define MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP

#ifdef _MSC_VER
#ifdef matador_json_EXPORTS
    #define OOS_JSON_API __declspec(dllexport)
    #define EXPIMP_JSON_TEMPLATE
  #else
    #define OOS_JSON_API __declspec(dllimport)
    #define EXPIMP_JSON_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_JSON_API
#endif

#include "matador/utils/serializer.hpp"

namespace matador {

/// @cond MATADOR_DEV

class basic_identifier;

class OOS_JSON_API json_identifier_serializer : public serializer
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

/// @endcond

}

#endif //MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
