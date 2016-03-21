//
// Created by sascha on 3/21/16.
//

#include "orm/table.hpp"

namespace oos {

std::string table::name() const
{
  return name_;
}

void table::create(connection &conn)
{
  create_(name_, conn);
}

void table::drop(connection &conn)
{
  drop_(name_, conn);
}

}