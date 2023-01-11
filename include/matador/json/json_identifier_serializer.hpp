#ifndef MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
#define MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP

#include "matador/json/export.hpp"

#include <string>

namespace matador {

/// @cond MATADOR_DEV

class basic_identifier;
class time;
class date;
class identifiable_holder;
class abstract_has_many;

enum cascade_type;

class OOS_JSON_API json_identifier_serializer
{
public:
  json_identifier_serializer() = default;

  std::string serialize(basic_identifier &x);

  void on_attribute(const char*, char&);
  void on_attribute(const char*, short&);
  void on_attribute(const char*, int&);
  void on_attribute(const char*, long&);
  void on_attribute(const char*, long long&);
  void on_attribute(const char*, unsigned char&);
  void on_attribute(const char*, unsigned short&);
  void on_attribute(const char*, unsigned int&);
  void on_attribute(const char*, unsigned long&);
  void on_attribute(const char*, unsigned long long&);
  void on_attribute(const char*, bool&);
  void on_attribute(const char*, float&);
  void on_attribute(const char*, double&);
  void on_attribute(const char*, char *, size_t);
  void on_attribute(const char*, std::string&);
  void on_attribute(const char*, std::string&, size_t);
  void on_attribute(const char*, matador::time&) {}
  void on_attribute(const char*, matador::date&) {}
  void on_belongs_to(const char*, matador::basic_identifier &) {}
  void on_has_one(const char*, matador::identifiable_holder &, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  std::string json_;
};

/// @endcond

}

#endif //MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
