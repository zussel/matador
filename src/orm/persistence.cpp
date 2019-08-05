//
// Created by sascha on 20.03.16.
//

#include "matador/orm/persistence.hpp"

namespace matador {


persistence::persistence(const std::string &dns)
  : connection_(dns)
{
  connection_.connect();
}

persistence::~persistence()
{
  tables_.clear();
  connection_.disconnect();
}

void persistence::detach(const char *type)
{
  store_.detach(type);
}

void persistence::create()
{
  for (t_table_map::value_type &val : tables_) {
    if (!connection_.exists(val.second->name())) {
      val.second->create(connection_);
    }
    val.second->prepare(connection_);
  }
}

void persistence::drop()
{
  for (t_table_map::value_type &val : tables_) {
    if (!connection_.exists(val.second->name())) {
      continue;
    }
    val.second->drop(connection_);
  }
}

void persistence::clear()
{
  store_.clear();
  for (t_table_map::value_type &val : tables_) {
    val.second->reset();
  }
}

persistence::t_table_map::iterator persistence::find_table(const std::string &type)
{
  return tables_.find(type);
}

persistence::t_table_map::iterator persistence::begin()
{
  return tables_.begin();
}

persistence::t_table_map::iterator persistence::end()
{
  return tables_.end();
}

object_store &persistence::store()
{
  return store_;
}

const object_store &persistence::store() const
{
  return store_;
}

connection &persistence::conn()
{
  return connection_;
}

const connection &persistence::conn() const {
  return connection_;
}

void persistence::enable_log()
{
  connection_.enable_log();
}

void persistence::disable_log()
{
  connection_.disable_log();
}

bool persistence::is_log_enabled() const
{
  return connection_.is_log_enabled();
}

}