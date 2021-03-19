/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "matador/sql/connection_factory.hpp"
#include "matador/sql/memory_connection.hpp"

#include <cstdint>

namespace matador {

connection_factory::connection_factory()
{
  std::unique_ptr<factory_t::producer_base> dbp(new connection_producer);
  factory_.insert("memory", dbp.release());
}

connection_impl* connection_factory::create(const std::string &name)
{
  auto i = factory_.find(name);
  if (i == factory_.end()) {
    std::unique_ptr<factory_t::producer_base> producer(new dynamic_connection_producer(name));
    i = factory_.insert(name, producer.release()).first;
  }
  auto *producer = static_cast<connection_producer*>(i->second.get());
  return producer->create();
}

bool connection_factory::destroy(const std::string &name, connection_impl* impl)
{
  auto i = factory_.find(name);
  if (i == factory_.end()) {
    // couldn't find sql backend
    return false;
  }
  auto *producer = static_cast<connection_producer*>(i->second.get());
  producer->destroy(impl);
  return true;
}

connection_impl* connection_factory::connection_producer::create() const
{
  return new memory::memory_connection;
}

void connection_factory::connection_producer::destroy(connection_impl* val) const
{
  delete val;
}

connection_factory::dynamic_connection_producer::dynamic_connection_producer(const std::string &name)
{
  // load matador driver library
  // create instance
  if (!loader_.load(("matador-"+name))) {
    throw std::runtime_error("couldn't find library [" + name + "]");
  }
  // get create function
  create_ = reinterpret_cast<create_func>(reinterpret_cast<std::uintptr_t>(loader_.function("create_database")));
  // get destroy function
  destroy_ = reinterpret_cast<destroy_func>(reinterpret_cast<std::uintptr_t>(loader_.function("destroy_database")));
}

connection_factory::dynamic_connection_producer::~dynamic_connection_producer()
{
  loader_.unload();
}

connection_impl* connection_factory::dynamic_connection_producer::create() const
{
  // on each call store the created sql for later
  // explicit destruction
  return (*create_)();
}

void connection_factory::dynamic_connection_producer::destroy(connection_impl* val) const
{
  (*destroy_)(val);
}

}
