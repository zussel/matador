//
// Created by sascha on 12.01.20.
//

#ifndef MATADOR_JSON_IDENTIFIER_MAPPER_HPP
#define MATADOR_JSON_IDENTIFIER_MAPPER_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include <utility>
#include <ostream>

#include "matador/utils/serializer.hpp"
#include "matador/utils/json.hpp"

namespace matador {

class OOS_UTILS_API json_identifier_mapper : public serializer
{
public:
  void set_identifier_value(basic_identifier &id, const json &val);
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
  void serialize(const char*, bool&) override {}
  void serialize(const char*, float&) override {}
  void serialize(const char*, double&) override {}
  void serialize(const char*, char *, size_t) override;
  void serialize(const char*, std::string&) override {}
  void serialize(const char*, std::string&, size_t) override;
  void serialize(const char*, matador::time&) override {}
  void serialize(const char*, matador::date&) override {}
  void serialize(const char*, matador::basic_identifier &) override {}
  void serialize(const char*, matador::identifiable_holder &, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, cascade_type) override {}

private:
  json value_;
};

}
#endif //MATADOR_JSON_IDENTIFIER_MAPPER_HPP
