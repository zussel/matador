//
// Created by sascha on 3/2/16.
//
#include "sql/value.hpp"

namespace oos {
namespace detail {

std::string basic_value::compile(basic_dialect &d) const
{
  return d.compile(*this);
}

}

std::string null_value::NULLSTR = "NULL";

void null_value::serialize(const char *id, serializer &srlzr)
{
  srlzr.serialize(id, NULLSTR);
}

std::string null_value::str() const
{
  return NULLSTR;
}

detail::basic_value* make_value(const char* val, size_t len)
{
  return new value<char*>(val, len);
}

}