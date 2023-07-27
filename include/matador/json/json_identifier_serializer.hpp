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

  void serialize(short &x, long size) override;
  void serialize(int &x, long size) override;
  void serialize(long &x, long size) override;
  void serialize(long long &x, long size) override;
  void serialize(unsigned short &x, long size) override;
  void serialize(unsigned int &x, long size) override;
  void serialize(unsigned long &x, long size) override;
  void serialize(unsigned long long &x, long size) override;
  void serialize(std::string &, long size) override;
  void serialize(null_type_t &, long size) override;

private:
  std::string json_;
};

/// @endcond

}

#endif //MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
