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

  void serialize(const char*, char&);
  void serialize(const char*, short&);
  void serialize(const char*, int&);
  void serialize(const char*, long&);
  void serialize(const char*, long long&);
  void serialize(const char*, unsigned char&);
  void serialize(const char*, unsigned short&);
  void serialize(const char*, unsigned int&);
  void serialize(const char*, unsigned long&);
  void serialize(const char*, unsigned long long&);
  void serialize(const char*, bool&);
  void serialize(const char*, float&);
  void serialize(const char*, double&);
  void serialize(const char*, char *, size_t);
  void serialize(const char*, std::string&);
  void serialize(const char*, std::string&, size_t);
  void serialize(const char*, matador::time&) {}
  void serialize(const char*, matador::date&) {}
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
