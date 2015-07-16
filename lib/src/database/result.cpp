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

void result::read_foreign_key(const char *id, object_base_ptr &x)
{
  std::shared_ptr<primary_key_base> pk(x.proxy_->pk());
  pk->deserialize(id, *this);
}

}
