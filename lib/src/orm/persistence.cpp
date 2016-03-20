//
// Created by sascha on 20.03.16.
//

#include "orm/persistence.hpp"

namespace oos {


persistence::persistence(const std::string &dns)
  : connection_(dns)
{
  connection_.open();
}

persistence::~persistence()
{
  connection_.close();
}

void persistence::create()
{
  // Todo: create all tables
}
void persistence::drop()
{
  // Todo: drop all tables
}

}