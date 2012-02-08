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

namespace oos {

result_impl::~result_impl()
{}

void result_impl::push_back(row *r)
{
  rows_.push_back(r);
}

result_iterator::result_iterator()
  : res_(0)
{}

result_iterator::result_iterator(result *res)
  : res_(res)
{}

result_iterator::result_iterator(const result_iterator &x)
  : res_(x.res_)
{}

result_iterator& result_iterator::operator=(const result_iterator &x)
{
  res_ = x.res_;
  return *this;
}

result_iterator::~result_iterator()
{}

result_iterator::self& result_iterator::operator++()
{
  if (res_ && !res_->step()) {
    res_ = 0;
  }
  return *this;
}

result_iterator::self result_iterator::operator++(int)
{
  result_iterator tmp(*this);
  ++(*this);
  return tmp;
}

result_iterator::pointer result_iterator::operator->() const
{
  return &(operator *());
}

result_iterator::reference result_iterator::operator*() const
{
  return (*res_->current());
}

bool result_iterator::operator==(const self &x) const
{
  return true;
}

bool result_iterator::operator!=(const self &x) const
{
  return false;
}

result::result(statement *stmt)
  : stmt_(stmt)
  , current_(0)
{}

result::~result()
{}

bool result::step()
{
  current_ = stmt_->step();
  return true;
}

row* result::current() const
{
  return 0;
}

result_iterator result::begin()
{
  return result_iterator(this);
}

result_iterator result::end()
{
  return result_iterator();
}

}
