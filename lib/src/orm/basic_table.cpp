//
// Created by sascha on 3/24/16.
//

#include "orm/basic_table.hpp"

namespace oos {

basic_table::basic_table(const std::string &name, t_create_func create_func, t_drop_func drop_func,
                         t_object_func insert_object_func, t_object_func update_object_func, t_object_func delete_object_func)
  : name_(name)
  , create_(create_func)
  , drop_(drop_func)
  , insert_object_(insert_object_func)
  , update_object_(update_object_func)
  , delete_object_(delete_object_func)
{ }

std::string basic_table::name() const
{
  return name_;
}

void basic_table::create(connection &conn)
{
  create_(name_, conn);
}

void basic_table::drop(connection &conn)
{
  drop_(name_, conn);
}

void basic_table::insert(object_proxy *proxy)
{
  insert_object_(proxy);
}

void basic_table::update(object_proxy *proxy)
{
  update_object_(proxy);
}

void basic_table::remove(object_proxy *proxy)
{
  delete_object_(proxy);
}

}