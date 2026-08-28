#ifndef MATADOR_ATTRIBUTE_BINDER_HPP
#define MATADOR_ATTRIBUTE_BINDER_HPP

#include "matador/utils/types.hpp"

#include <string>

namespace matador::utils {

class column_value;

class value_writer
{
public:
  virtual ~value_writer() = default;

  virtual void write_value(size_t pos, const int8_t &x) = 0;
  virtual void write_value(size_t pos, const int16_t &x) = 0;
  virtual void write_value(size_t pos, const int32_t &x) = 0;
  virtual void write_value(size_t pos, const int64_t &x) = 0;
  virtual void write_value(size_t pos, const uint8_t &x) = 0;
  virtual void write_value(size_t pos, const uint16_t &x) = 0;
  virtual void write_value(size_t pos, const uint32_t &x) = 0;
  virtual void write_value(size_t pos, const uint64_t &x) = 0;
  virtual void write_value(size_t pos, const bool &x) = 0;
  virtual void write_value(size_t pos, const float &x) = 0;
  virtual void write_value(size_t pos, const double &x) = 0;
  virtual void write_value(size_t pos, const date_type_t &x) = 0;
  virtual void write_value(size_t pos, const time_type_t &x) = 0;
  virtual void write_value(size_t pos, const timestamp_type_t &x) = 0;
  virtual void write_value(size_t pos, const char *x) = 0;
  virtual void write_value(size_t pos, const char *x, size_t size) = 0;
  virtual void write_value(size_t pos, const std::string &x) = 0;
  virtual void write_value(size_t pos, const std::string &x, size_t size) = 0;
  virtual void write_value(size_t pos, const blob_type_t &x) = 0;
  virtual void write_value(size_t pos, const column_value &x, size_t size) = 0;
};

}

#endif //MATADOR_ATTRIBUTE_BINDER_HPP
