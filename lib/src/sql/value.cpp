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
}