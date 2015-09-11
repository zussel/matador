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
#include "object/prototype_tree.hpp"

namespace oos {


result_iterator::result_iterator()
{

}

result_iterator::result_iterator(const result_iterator &x)
{

}

result_iterator &result_iterator::operator=(const result_iterator &x)
{
  return *this;
}

result_iterator::~result_iterator()
{

}

bool result_iterator::operator==(const result_iterator &rhs)
{
  return false;
}

bool result_iterator::operator!=(const result_iterator &rhs)
{
  return false;
}

result_iterator &result_iterator::operator++()
{
  return *this;
}

result_iterator result_iterator::operator++(int i)
{
  return oos::result_iterator();
}

result_iterator::pointer result_iterator::operator->()
{
  return nullptr;
}

result_iterator::pointer result_iterator::operator&()
{
  return nullptr;
}

result_iterator::reference result_iterator::operator*()
{
  return *obj;
}

result_iterator::pointer result_iterator::get() {
  return nullptr;
}

result_iterator::pointer result_iterator::release()
{
  return nullptr;
}

result::result()
{

}

result::~result()
{

}

result::result(const result &x)
  : p(x.p)
{
}

result &result::operator=(const result &x)
{
  p = x.p;
  return *this;
}

result::iterator result::begin()
{
  return oos::result_iterator();
}

result::iterator result::end()
{
  return oos::result_iterator();
}

bool result::empty() const
{
  return false;
}

std::size_t result::size() const
{
  return 0;
}


result::result(result::result_impl *impl)
  : p(impl)
{ }

template<class T>
result result::create_result(T *impl) const {
  return oos::result(impl);
}

}
