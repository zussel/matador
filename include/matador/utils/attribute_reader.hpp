#ifndef MATADOR_ATTRIBUTE_READER_HPP
#define MATADOR_ATTRIBUTE_READER_HPP

#include "matador/utils/types.hpp"

#include <string>

namespace matador::utils {

class value;

class attribute_reader
{
public:
  virtual ~attribute_reader() = default;

  virtual void read_value(const char *id, size_t index, int8_t &value) = 0;
  virtual void read_value(const char *id, size_t index, int16_t &value) = 0;
  virtual void read_value(const char *id, size_t index, int32_t &value) = 0;
  virtual void read_value(const char *id, size_t index, int64_t &value) = 0;
  virtual void read_value(const char *id, size_t index, uint8_t &value) = 0;
  virtual void read_value(const char *id, size_t index, uint16_t &value) = 0;
  virtual void read_value(const char *id, size_t index, uint32_t &value) = 0;
  virtual void read_value(const char *id, size_t index, uint64_t &value) = 0;
  virtual void read_value(const char *id, size_t index, bool &value) = 0;
  virtual void read_value(const char *id, size_t index, float &value) = 0;
  virtual void read_value(const char *id, size_t index, double &value) = 0;
  virtual void read_value(const char *id, size_t index, time_type_t &value) = 0;
  virtual void read_value(const char *id, size_t index, date_type_t &value) = 0;
  virtual void read_value(const char *id, size_t index, timestamp_type_t &value) = 0;
  virtual void read_value(const char *id, size_t index, char *value, size_t size) = 0;
  virtual void read_value(const char *id, size_t index, std::string &value) = 0;
  virtual void read_value(const char *id, size_t index, std::string &value, size_t size) = 0;
  virtual void read_value(const char *id, size_t index, blob_type_t &value) = 0;
  virtual void read_value(const char *id, size_t index, value &value, size_t size) = 0;
};

}

#endif //MATADOR_ATTRIBUTE_READER_HPP
