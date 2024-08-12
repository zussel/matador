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

  virtual void bind(size_t pos, char x) = 0;
  virtual void bind(size_t pos, short x) = 0;
  virtual void bind(size_t pos, int x) = 0;
  virtual void bind(size_t pos, long x) = 0;
  virtual void bind(size_t pos, long long x) = 0;
  virtual void bind(size_t pos, unsigned char x) = 0;
  virtual void bind(size_t pos, unsigned short x) = 0;
  virtual void bind(size_t pos, unsigned int x) = 0;
  virtual void bind(size_t pos, unsigned long x) = 0;
  virtual void bind(size_t pos, unsigned long long x) = 0;
  virtual void bind(size_t pos, bool x) = 0;
  virtual void bind(size_t pos, float x) = 0;
  virtual void bind(size_t pos, double x) = 0;
  virtual void bind(size_t pos, const matador::time &x) = 0;
  virtual void bind(size_t pos, const matador::date &x) = 0;
  virtual void bind(size_t pos, const char *x) = 0;
  virtual void bind(size_t pos, const char *x, size_t size) = 0;
  virtual void bind(size_t pos, const std::string &x) = 0;
  virtual void bind(size_t pos, const std::string &x, size_t size) = 0;
  virtual void bind(size_t pos, const utils::blob &x) = 0;
};

}

#endif //MATADOR_ATTRIBUTE_BINDER_HPP
