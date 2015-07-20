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

#include "database/result.hpp"
#include "database/statement.hpp"

#include "object/serializable.hpp"
#include "object/object_ptr.hpp"
#include "object/object_exception.hpp"
#include "object/primary_key.hpp"

namespace oos {

result::result()
{}

result::~result()
{}

void result::get(serializable *o)
{
  result_index = transform_index(0);
  o->deserialize(*this);
}

serializable* result::fetch(const oos::prototype_node *node)
{
  node_ = node;

  std::unique_ptr<serializable> obj(node_->producer->create());

  if (!fetch(obj.get())) {
    obj.reset(nullptr);
  }

  node_ = nullptr;
  return obj.release();
}

void result::read_foreign_key(const char *id, object_base_ptr &x)
{

  prototype_node::t_foreign_key_map::const_iterator i = node_->foreign_keys.find(id);
  if (i == node_->foreign_keys.end()) {
    throw_object_exception("couldn't find foreign key for serializable of type'" << x.type() << "'");
  }

  /*
   * clone new primary key and deserialize it
   * if valid value is set create new proxy
   * with primary key
   */
  std::shared_ptr<primary_key_base> pk(i->second->clone());
  pk->deserialize(id, *this);
  if (!pk->is_valid()) {
    return;
  }


  std::unique_ptr<object_proxy> proxy(new object_proxy(nullptr));
  // Todo: create a object_proxy ctor like below
//  std::unique_ptr<object_proxy> proxy(new object_proxy(const_cast<prototype_node*>(&node_), pk));

  proxy->primary_key_ = pk;
  proxy->node_ = const_cast<prototype_node*>(node_);

  x.reset(proxy.release());
}

}
