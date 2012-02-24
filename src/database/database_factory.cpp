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
#include <iostream>

using std::cout;

namespace oos {

database_factory::database_factory()
{}

database_factory::~database_factory()
{
  for (factory_t::iterator i = factory_.begin(); i != factory_.end(); ++i) {
    delete i->second;
  }
}

database_impl* database_factory::create(const std::string &name, database *db)
{
  factory_t::iterator i = factory_.find(name);
  if (i == factory_.end()) {
    database_loader *loader = new database_loader(name);
    i = factory_.insert(std::make_pair(name, loader)).first;
  }
  database_impl *impl = i->second->create("");
  impl->initialize(db);
  return impl;
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

database_impl* database_factory::database_loader::create(const std::string &conn) const
{
  // on each call store the created database for later
  // explicit destruction
  return producer_->create(conn);
}

}
