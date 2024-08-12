#ifndef MATADOR_ATTRIBUTE_READER_HPP
#define MATADOR_ATTRIBUTE_READER_HPP

#include "matador/utils/types.hpp"

#include <cstdlib>
#include <string>

namespace matador {
class date;
class time;
}
namespace matador::object {

class attribute_reader
{
public:
  virtual ~attribute_reader() = default;

  virtual void read_value(const char *id, size_t index, char &value) = 0;
  virtual void read_value(const char *id, size_t index, short &value) = 0;
  virtual void read_value(const char *id, size_t index, int &value) = 0;
  virtual void read_value(const char *id, size_t index, long &value) = 0;
  virtual void read_value(const char *id, size_t index, long long &value) = 0;
  virtual void read_value(const char *id, size_t index, unsigned char &value) = 0;
  virtual void read_value(const char *id, size_t index, unsigned short &value) = 0;
  virtual void read_value(const char *id, size_t index, unsigned int &value) = 0;
  virtual void read_value(const char *id, size_t index, unsigned long &value) = 0;
  virtual void read_value(const char *id, size_t index, unsigned long long &value) = 0;
  virtual void read_value(const char *id, size_t index, bool &value) = 0;
  virtual void read_value(const char *id, size_t index, float &value) = 0;
  virtual void read_value(const char *id, size_t index, double &value) = 0;
  virtual void read_value(const char *id, size_t index, matador::time &value) = 0;
  virtual void read_value(const char *id, size_t index, matador::date &value) = 0;
  virtual void read_value(const char *id, size_t index, char *value, size_t s) = 0;
  virtual void read_value(const char *id, size_t index, std::string &value) = 0;
  virtual void read_value(const char *id, size_t index, std::string &value, size_t s) = 0;
  virtual void read_value(const char *id, size_t index, utils::blob &value) = 0;
};

}

#endif //MATADOR_ATTRIBUTE_READER_HPP
