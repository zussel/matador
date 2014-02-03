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

#include "tools/sequencer.hpp"

namespace oos {

default_sequencer::default_sequencer()
  : number_(0)
{}

default_sequencer::~default_sequencer()
{}

long default_sequencer::init()
{
  return number_;
}

long default_sequencer::reset(long id)
{
  number_ = id;
  return number_;
}

long default_sequencer::next()
{
  return ++number_;
}

long default_sequencer::current() const
{
  return number_;
}

long default_sequencer::update(long id)
{
  if (id > number_) {
    number_ = id;
  }
  return number_;
}

sequencer::sequencer(const sequencer_impl_ptr &impl)
  : impl_(impl)
{
}

sequencer::~sequencer()
{}

sequencer_impl_ptr sequencer::exchange_sequencer(const sequencer_impl_ptr &impl)
{
  sequencer_impl_ptr tmp = impl_;
  impl_ = impl;
  return tmp;
}

long sequencer::init()
{
  return impl_->init();
}

long sequencer::reset(long id)
{
  return impl_->reset(id);
}

long sequencer::next()
{
  return impl_->next();
}

long sequencer::current() const
{
  return impl_->current();
}

long sequencer::update(long id)
{
  return impl_->update(id);
}

}
