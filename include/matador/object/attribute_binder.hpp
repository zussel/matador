#ifndef MATADOR_ATTRIBUTE_BINDER_HPP
#define MATADOR_ATTRIBUTE_BINDER_HPP

#include "matador/utils/types.hpp"

#include <cstdlib>
#include <string>

namespace matador {
class date;
class time;
}
namespace matador::object {

class attribute_binder
{
public:
  virtual ~attribute_binder() = default;

  virtual void bind(size_t pos, const char &x) = 0;
  virtual void bind(size_t pos, const short &x) = 0;
  virtual void bind(size_t pos, const int &x) = 0;
  virtual void bind(size_t pos, const long &x) = 0;
  virtual void bind(size_t pos, const long long &x) = 0;
  virtual void bind(size_t pos, const unsigned char &x) = 0;
  virtual void bind(size_t pos, const unsigned short &x) = 0;
  virtual void bind(size_t pos, const unsigned int &x) = 0;
  virtual void bind(size_t pos, const unsigned long &x) = 0;
  virtual void bind(size_t pos, const unsigned long long &x) = 0;
  virtual void bind(size_t pos, const bool &x) = 0;
  virtual void bind(size_t pos, const float &x) = 0;
  virtual void bind(size_t pos, const double &x) = 0;
  virtual void bind(size_t pos, const time &x) = 0;
  virtual void bind(size_t pos, const date &x) = 0;
  virtual void bind(size_t pos, const char *x) = 0;
  virtual void bind(size_t pos, const char *x, size_t size) = 0;
  virtual void bind(size_t pos, const std::string &x) = 0;
  virtual void bind(size_t pos, const std::string &x, size_t size) = 0;
  virtual void bind(size_t pos, const utils::blob &x) = 0;
};

}

#endif //MATADOR_ATTRIBUTE_BINDER_HPP
