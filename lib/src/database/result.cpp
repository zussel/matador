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
#include "database/result_impl.hpp"
#include "database/statement.hpp"

#include "object/serializable.hpp"
#include "object/object_ptr.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_tree.hpp"

namespace oos {


result_iterator::result_iterator() { }

result_iterator::result_iterator(oos::detail::result_impl *result_impl, oos::object_base_producer *producer, serializable *obj)
  : obj_(obj)
  , result_impl_(result_impl)
  , producer_(producer)
{

}

result_iterator::result_iterator(const result_iterator &x)
  : result_impl_(x.result_impl_)
  , producer_(x.producer_)
{ }

result_iterator &result_iterator::operator=(const result_iterator &x)
{
  result_impl_ = x.result_impl_;
  producer_ = x.producer_;
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
  obj_.reset(producer_->create());
  result_impl_->fetch(obj_.get());
  return *this;
}

result_iterator result_iterator::operator++(int i)
{
  serializable *obj = producer_->create();
  result_impl_->fetch(obj);
  return oos::result_iterator(result_impl_, producer_, obj);
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

result::result(oos::detail::result_impl *impl, object_base_producer *producer)
    : p(impl)
    , producer_(producer)
{ }

result::~result()
{
  if (p) {
    delete p;
  }
}

result::result(result &&x)
  : p(x.p)
  , producer_(x.producer_)
{
  x.p = nullptr;
}

result &result::operator=(result &&x)
{
  p = x.p;
  producer_ = x.producer_;
  x.p = nullptr;
  return *this;
}

result::iterator result::begin()
{
  return ++oos::result_iterator(p, producer_);
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
