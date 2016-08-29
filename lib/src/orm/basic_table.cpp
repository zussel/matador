//
// Created by sascha on 3/24/16.
//

#include <orm/persistence.hpp>
#include "orm/basic_table.hpp"

namespace oos {

basic_table::basic_table(prototype_node *node, persistence &p)
  : persistence_(p)
  , node_(node)
{ }

basic_table::~basic_table() {}


std::string basic_table::name() const
{
  return node_->type();
}

bool basic_table::is_loaded() const
{
  return is_loaded_;
}

basic_table::t_table_map::iterator basic_table::find_table(const std::string &type)
{
  return persistence_.find_table(type);
}

basic_table::t_table_map::iterator basic_table::begin_table()
{
  return persistence_.begin();
}

basic_table::t_table_map::iterator basic_table::end_table()
{
  return persistence_.end();
}


}