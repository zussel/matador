#ifndef QUERY_PARAMETER_BINDER_HPP
#define QUERY_PARAMETER_BINDER_HPP

#include "matador/utils/types.hpp"

#include <string>
#include <cstring>

namespace matador::sql {

class parameter_binder
{
public:
  virtual ~parameter_binder() = default;

  virtual void bind(size_t pos, char) = 0;
  virtual void bind(size_t pos, short) = 0;
  virtual void bind(size_t pos, int) = 0;
  virtual void bind(size_t pos, long) = 0;
  virtual void bind(size_t pos, long long) = 0;
  virtual void bind(size_t pos, unsigned char) = 0;
  virtual void bind(size_t pos, unsigned short) = 0;
  virtual void bind(size_t pos, unsigned int) = 0;
  virtual void bind(size_t pos, unsigned long) = 0;
  virtual void bind(size_t pos, unsigned long long) = 0;
  virtual void bind(size_t pos, bool) = 0;
  virtual void bind(size_t pos, float) = 0;
  virtual void bind(size_t pos, double) = 0;
  virtual void bind(size_t pos, const char *) = 0;
  virtual void bind(size_t pos, const char *, size_t size) = 0;
  virtual void bind(size_t pos, const std::string&) = 0;
  virtual void bind(size_t pos, const std::string &x, size_t size) = 0;
  virtual void bind(size_t pos, const utils::blob &) = 0;
//  virtual void bind(size_t pos, const matador::time&) = 0;
//  virtual void bind(size_t pos, const matador::date&) = 0;
};

}

#endif //QUERY_PARAMETER_BINDER_HPP
