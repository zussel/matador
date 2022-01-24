#ifndef MATADOR_ANY_HPP
#define MATADOR_ANY_HPP

#include "matador/utils/serializer.hpp"

namespace matador {

class basic_embed
{
public:
  virtual ~basic_embed() = default;

  virtual void serialize(const char *id, serializer &s) = 0;
};

template < typename T >
class embed : public basic_embed
{
public:
  using value_type = T;

  T& operator()() { return value; }
  const T& operator()() const { return value; }

  T* operator->() { return &value; }
  const T* operator->() const { return &value; }

  void serialize(const char *, serializer &s) override
  {
    value.serialize(s);
  }

private:
  value_type value;
};



}

#endif /* MATADOR_ANY_HPP */