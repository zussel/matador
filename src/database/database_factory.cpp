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
#include "database/database.hpp"
#include "database/database_impl.hpp"

#include <stdexcept>

namespace oos {

database_factory::database_factory()
{}

database_factory::~database_factory()
{}

database_impl* database_factory::create(const std::string &name, database *db)
{
  factory_t::iterator i = factory_.find(name);
  if (i != factory_.end()) {
    database_impl *impl = i->second->create();
    impl->initialize(db);
    return impl;
  } else {
    factory_.insert(name, new database_producer(name));
    return 0;
  }
}

database_factory::database_producer::database_producer(const std::string &name)
{
  // load oos driver library
  // create instance
  if (!loader_.load(name.c_str())) {
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

database_impl* database_factory::database_producer::create() const
{
  // on each call store the created database for later
  // explicit destruction
  return (*create_)("");
}

}
