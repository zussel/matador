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

#include "database/database_factory.hpp"
#include "database/session.hpp"
#include "database/database.hpp"
#include "database/memory_database.hpp"

namespace oos {

database_factory::database_factory()
{
  std::unique_ptr<factory_t::producer_base> dbp(new database_producer);
  factory_.insert("memory", dbp.release());
}

database_factory::~database_factory()
{}

database* database_factory::create(const std::string &name, session *db)
{
  factory_t::iterator i = factory_.find(name);
  if (i == factory_.end()) {
    std::unique_ptr<factory_t::producer_base> producer(new dynamic_database_producer(name));
    i = factory_.insert(name, producer.release()).first;
  }
  database_producer *producer = static_cast<database_producer*>(i->second.get());
  return producer->create(db);
}

bool database_factory::destroy(const std::string &name, database* impl)
{
  factory_t::iterator i = factory_.find(name);
  if (i == factory_.end()) {
    // couldn't find sql backend
    return false;
  }
  database_producer *producer = static_cast<database_producer*>(i->second.get());
  producer->destroy(impl);
  return true;
}

database* database_factory::database_producer::create() const
{
  return new memory_database(db_);
}

database* database_factory::database_producer::create(session *db)
{
  db_ = db;
  std::unique_ptr<database> impl(create());
  db_ = nullptr;
  return impl.release();
}
void database_factory::database_producer::destroy(database* val) const
{
  delete val;
}

database_factory::dynamic_database_producer::dynamic_database_producer(const std::string &name)
{
  // load oos driver library
  // create instance
  if (!loader_.load(("oos-"+name).c_str())) {
    throw std::runtime_error("couldn't fínd library [" + name + "]");
  }
  // get create function
  create_ = reinterpret_cast<create_func>(reinterpret_cast<long>(loader_.function("create_database")));
  // get destroy function
  destroy_ = reinterpret_cast<destroy_func>(reinterpret_cast<long>(loader_.function("destroy_database")));
}

database_factory::dynamic_database_producer::~dynamic_database_producer()
{
  loader_.unload();
}

database* database_factory::dynamic_database_producer::create() const
{
  // on each call store the created sql for later
  // explicit destruction
  return (*create_)(db_);
}

void database_factory::dynamic_database_producer::destroy(database* val) const
{
  (*destroy_)(val);
}

}
