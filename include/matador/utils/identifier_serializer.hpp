#ifndef MATADOR_IDENTIFIER_SERIALIZER_HPP
#define MATADOR_IDENTIFIER_SERIALIZER_HPP

#include "matador/utils/types.hpp"

#include <cstdint>
#include <string>

namespace matador::utils {
class field_attributes;

class identifier_serializer {
public:
  virtual ~identifier_serializer() = default;

  virtual void serialize(int8_t &, const field_attributes &) = 0;
  virtual void serialize(int16_t &, const field_attributes &) = 0;
  virtual void serialize(int32_t &, const field_attributes &) = 0;
  virtual void serialize(int64_t &, const field_attributes &) = 0;
  virtual void serialize(uint8_t &, const field_attributes &) = 0;
  virtual void serialize(uint16_t &, const field_attributes &) = 0;
  virtual void serialize(uint32_t &, const field_attributes &) = 0;
  virtual void serialize(uint64_t &, const field_attributes &) = 0;
  virtual void serialize(std::string &, const field_attributes &) = 0;
  virtual void serialize(null_type_t &, const field_attributes &) = 0;
};
}
#endif //MATADOR_IDENTIFIER_SERIALIZER_HPP
