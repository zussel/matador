//
// Created by sascha on 3/24/16.
//

#include <orm/persistence.hpp>
#include "orm/basic_table.hpp"

namespace oos {

basic_table::basic_table(const std::string &name, persistence &p)
  : persistence_(p)
  , name_(name)
{ }

basic_table::~basic_table() {}


std::string basic_table::name() const
{
  return name_;
}

void basic_table::find_table(const std::string &type)
{
  return persistence_.find_table(type);
}

}