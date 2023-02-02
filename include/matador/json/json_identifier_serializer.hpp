#ifndef MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
#define MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP

#include "matador/json/export.hpp"

#include "matador/utils/identifier.hpp"

namespace matador {

/// @cond MATADOR_DEV

class time;
class date;
class identifiable_holder;
class abstract_has_many;

enum class cascade_type;

class OOS_JSON_API json_identifier_serializer : public identifier_serializer
{
public:
  json_identifier_serializer() = default;

  std::string serialize(const identifier &x);

  void serialize(const char *, short &x) override;
  void serialize(const char *, int &x) override;
  void serialize(const char *, long &x) override;
  void serialize(const char *, long long &x) override;
  void serialize(const char *, unsigned short &x) override;
  void serialize(const char *, unsigned int &x) override;
  void serialize(const char *, unsigned long &x) override;
  void serialize(const char *, unsigned long long &x) override;
  void serialize(const char *, std::string &) override;
  void serialize(const char *, null_type_t &) override;

private:
  std::string json_;
};

/// @endcond

}

#endif //MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
