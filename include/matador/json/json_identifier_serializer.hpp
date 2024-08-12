#ifndef MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
#define MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP

#include "matador/json/export.hpp"

#include "matador/object/identifier.hpp"

namespace matador {

/// @cond MATADOR_DEV

class time;
class date;
class identifiable_holder;
class abstract_container;

enum class cascade_type;

class OOS_JSON_API json_identifier_serializer : public identifier_serializer
{
public:
  json_identifier_serializer() = default;

  std::string serialize(const identifier &x);

  void serialize(short &x, const utils::field_attributes &attr) override;
  void serialize(int &x, const utils::field_attributes &attr) override;
  void serialize(long &x, const utils::field_attributes &attr) override;
  void serialize(long long &x, const utils::field_attributes &attr) override;
  void serialize(unsigned short &x, const utils::field_attributes &attr) override;
  void serialize(unsigned int &x, const utils::field_attributes &attr) override;
  void serialize(unsigned long &x, const utils::field_attributes &attr) override;
  void serialize(unsigned long long &x, const utils::field_attributes &attr) override;
  void serialize(std::string &, const utils::field_attributes &attr) override;
  void serialize(utils::null_type_t &, const utils::field_attributes &attr) override;

private:
  std::string json_;
};

/// @endcond

}

#endif //MATADOR_JSON_IDENTIFIER_SERIALIZER_HPP
