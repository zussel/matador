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

  void serialize(short &x) override;
  void serialize(int &x) override;
  void serialize(long &x) override;
  void serialize(long long &x) override;
  void serialize(unsigned short &x) override;
  void serialize(unsigned int &x) override;
  void serialize(unsigned long &x) override;
  void serialize(unsigned long long &x) override;
  void serialize(std::string &) override;
  void serialize(null_type_t &) override;

private:
  std::string json_;
};

/// @endcond

}

#endif //MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
