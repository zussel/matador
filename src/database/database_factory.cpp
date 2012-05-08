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

#include <stdexcept>
#include <iostream>

using std::cout;

namespace oos {

database_factory::database_factory()
{}

database_factory::~database_factory()
{
  /*
  for (factory_t::iterator i = factory_.begin(); i != factory_.end(); ++i) {
    delete i->second;
  }
  */
}

database* database_factory::create(const std::string &name, session *db)
{
  factory_t::iterator i = factory_.find(name);
  if (i == factory_.end()) {
    database_producer *producer = new database_producer(name);
    i = factory_.insert(name, producer).first;
  }
  database_producer *producer = static_cast<database_producer*>(i->second.get());
  return producer->create(db);
}

bool database_factory::destroy(const std::string &name, database* impl)
{
  factory_t::iterator i = factory_.find(name);
  if (i == factory_.end()) {
    // couldn't find database backend
    return false;
  }
  database_producer *producer = static_cast<database_producer*>(i->second.get());
  producer->destroy(impl);
  return true;
}

database_factory::database_producer::database_producer(const std::string &name)
  : db_(0)
{
  // load oos driver library
  // create instance
  if (!loader_.load(("oos-"+name).c_str())) {
    throw std::runtime_error("couldn't fínd library [" + name + "]");
  }
  // get create function
  create_ = reinterpret_cast<create_func>(loader_.function("create_database"));
  // get destroy function
  destroy_ = reinterpret_cast<destroy_func>(loader_.function("destroy_database"));
}

database_factory::database_producer::~database_producer()
{
  loader_.unload();
}

database* database_factory::database_producer::create(session *db)
{
  db_ = db;
  database *impl = this->create();
  db_ = 0;
  return impl;
}

database* database_factory::database_producer::create() const
{
  // on each call store the created database for later
  // explicit destruction
  return (*create_)(db_);
}

void database_factory::database_producer::destroy(database_factory::factory_t::value_type* val) const
{
//  cout << "detroying database backend\n";
  (*destroy_)(val);
}

/*
database* database_factory::create(const std::string &name, session *db)
{
  factory_t::iterator i = factory_.find(name);
  if (i == factory_.end()) {
    database_loader *loader = new database_loader(name);
    i = factory_.insert(std::make_pair(name, loader)).first;
  }
  return i->second->create(db);
}

database_factory::database_loader::database_loader(const std::string &name)
{
  // load oos driver library
  // create instance
  if (!loader_.load(("oos-"+name).c_str())) {
    throw std::runtime_error("couldn't fínd library [" + name + "]");
  }

  // get producer
  get_producer prod_func = reinterpret_cast<get_producer>(loader_.function("get_producer"));
  producer_ = (*prod_func)();
}

database_factory::database_loader::~database_loader()
{
//  loader_.unload();
}

database* database_factory::database_loader::create(session *db) const
{
  // on each call store the created database for later
  // explicit destruction
  return producer_->create(db);
}
*/
}
