//
// Created by sascha on 3/2/16.
//
#include "sql/value.hpp"

namespace oos {
namespace detail {

std::string basic_value::compile(basic_dialect &d, token::t_compile_type compile_type) const
{
  return d.compile(*this);
}

}
}