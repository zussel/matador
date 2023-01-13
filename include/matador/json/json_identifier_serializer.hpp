#ifndef MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
#define MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP

#include "matador/json/export.hpp"

#include "matador/utils/serializer.hpp"

namespace matador {

/// @cond MATADOR_DEV

class basic_identifier;
class time;
class date;
class identifiable_holder;
class abstract_has_many;

enum class cascade_type;

class OOS_JSON_API json_identifier_serializer : public serializer
{
public:
  json_identifier_serializer() = default;

  std::string serialize(basic_identifier &x);

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
  void on_attribute(const char*, matador::time&) override {}
  void on_attribute(const char*, matador::date&) override {}
  void on_primary_key(const char*, matador::basic_identifier &) override {}
  void on_belongs_to(const char*, matador::identifiable_holder &, cascade_type) override {}
  void on_has_one(const char*, matador::identifiable_holder &, cascade_type) override {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) override {}

private:
  std::string json_;
};

/// @endcond

}

#endif //MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
