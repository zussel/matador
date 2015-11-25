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

#include "object/object_container.hpp"
#include "object/object_store.hpp"

using namespace std;

namespace oos {

object_container::object_container()
  : ostore_(nullptr)
  , owner_(nullptr)
{}

object_proxy* object_container::proxy(const object_base_ptr &optr) const
{
  return optr.proxy_;
}

object_proxy* object_container::owner() const
{
  return owner_;
}

void object_container::owner(object_proxy *ownr)
{
  owner_ = ownr;
}

object_store* object_container::ostore() const
{
  return ostore_;
}

void object_container::mark_modified(object_proxy *proxy)
{
  if (proxy->obj()) {
    ostore_->mark_modified(proxy);
  }
}

void object_container::install(object_store *ostore)
{
  ostore_ = ostore;
}

void object_container::uninstall()
{
  ostore_ = 0;
}

}
