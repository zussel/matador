//
// Created by sascha on 12.01.20.
//

#ifndef MATADOR_JSON_IDENTIFIER_MAPPER_HPP
#define MATADOR_JSON_IDENTIFIER_MAPPER_HPP

#include <utility>

#include "matador/utils/serializer.hpp"

namespace matador {

struct value_t
{
  enum value_type {
    UNKNOWN, INTEGER, REAL, BOOL, STRING, NULL_VAL
  };
  value_t() = default;
  explicit value_t(std::string s) : type(STRING), str(std::move(s)) {}
  explicit value_t(long long i) : type(INTEGER), integer(i) {}
  explicit value_t(double d) : type(REAL), real(d) {}
  explicit value_t(bool b) : type(BOOL), boolean(b) {}

  void reset() {type = UNKNOWN;}
  void set(const std::string &s) { type = STRING, str = s; }
  void set(long long i) { type = INTEGER, integer = i; }
  void set(double d) { type = REAL, real = d; }
  void set(bool b) { type = BOOL, boolean = b; }
  void set_null() { type = NULL_VAL; }

  value_type type = UNKNOWN;
  long long integer = 0;
  double real = 0.0;
  bool boolean = false;
  std::string str;

};
class json_identifier_mapper : public serializer
{
public:
  void set_identifier_value(basic_identifier &id, const value_t *val);
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
  const value_t *value_ = nullptr;
};

}
#endif //MATADOR_JSON_IDENTIFIER_MAPPER_HPP
