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

#include <iostream>
#include "database/result.hpp"
#include "database/result_impl.hpp"
#include "database/statement.hpp"

#include "object/serializable.hpp"
#include "object/object_ptr.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_tree.hpp"

namespace oos {


result_iterator::result_iterator() { }

result_iterator::result_iterator(oos::detail::result_impl *result_impl, serializable *obj)
  : obj_(obj)
  , result_impl_(result_impl)
{

}

result_iterator::result_iterator(result_iterator &&x)
  : obj_(x.obj_.release())
  , result_impl_(x.result_impl_)
{ }

result_iterator &result_iterator::operator=(result_iterator &&x)
{
  result_impl_ = x.result_impl_;
  obj_.reset(x.obj_.release());
  return *this;
}

result_iterator::~result_iterator()
{

}

bool result_iterator::operator==(const result_iterator &rhs)
{
  return obj_ == rhs.obj_;
}

bool result_iterator::operator!=(const result_iterator &rhs)
{
  return obj_ != rhs.obj_;
}

result_iterator &result_iterator::operator++()
{
  obj_.reset(result_impl_->producer()->create());
  if (!result_impl_->fetch(obj_.get())) {
    obj_.reset();
  }
  return *this;
}

result_iterator result_iterator::operator++(int)
{
  std::unique_ptr<serializable> obj(result_impl_->producer()->create());
  result_impl_->fetch(obj.get());
  return oos::result_iterator(result_impl_, obj.release());
}

result_iterator::pointer result_iterator::operator->()
{
  return obj_.get();
}

result_iterator::pointer result_iterator::operator&()
{
  return obj_.get();
}

result_iterator::reference result_iterator::operator*()
{
  return *obj_.get();
}

result_iterator::pointer result_iterator::get() {
  return obj_.get();
}

result_iterator::pointer result_iterator::release()
{
  return obj_.release();
}

result::result() { }

result::result(oos::detail::result_impl *impl, database *db)
  : p(impl)
  , db_(db)
{ }

result::~result()
{
  if (p && db_) {
    db_->free_result(p);
  }
}

result::result(result &&x)
{
  std::swap(p, x.p);
}

result &result::operator=(result &&x)
{
  if (p) {
    db_->free_result(p);
    p = nullptr;
  }
  std::swap(p, x.p);
  return *this;
}

result::iterator result::begin()
{
  return std::move(++oos::result_iterator(p));
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

}
