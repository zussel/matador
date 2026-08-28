#ifndef MATADOR_IDENTIFIER_SERIALIZER_HPP
#define MATADOR_IDENTIFIER_SERIALIZER_HPP

#include "matador/utils/types.hpp"

#include <cstdint>
#include <string>

namespace matador::query {
class column_options;

class identifier_serializer {
public:
  virtual ~identifier_serializer() = default;

  virtual void serialize(int8_t &, const column_options &) = 0;
  virtual void serialize(int16_t &, const column_options &) = 0;
  virtual void serialize(int32_t &, const column_options &) = 0;
  virtual void serialize(int64_t &, const column_options &) = 0;
  virtual void serialize(uint8_t &, const column_options &) = 0;
  virtual void serialize(uint16_t &, const column_options &) = 0;
  virtual void serialize(uint32_t &, const column_options &) = 0;
  virtual void serialize(uint64_t &, const column_options &) = 0;
  virtual void serialize(std::string &, const column_options &) = 0;
  virtual void serialize(utils::null_type_t &, const column_options &) = 0;
};
}
#endif //MATADOR_IDENTIFIER_SERIALIZER_HPP
