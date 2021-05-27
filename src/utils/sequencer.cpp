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

#include "matador/utils/sequencer.hpp"

#include <utility>

namespace matador {

unsigned long default_sequencer::init()
{
  return number_;
}

unsigned long default_sequencer::reset(unsigned long id)
{
  number_ = id;
  return number_;
}

unsigned long default_sequencer::next()
{
  return ++number_;
}

unsigned long default_sequencer::current() const
{
  return number_;
}

unsigned long default_sequencer::update(unsigned long id)
{
  if (id > number_) {
    number_ = id;
  }
  return number_;
}

sequencer::sequencer(sequencer_impl_ptr impl)
  : impl_(std::move(impl))
{
}

sequencer_impl_ptr sequencer::exchange_sequencer(const sequencer_impl_ptr &impl)
{
  sequencer_impl_ptr tmp = impl_;
  impl_ = impl;
  return tmp;
}

unsigned long sequencer::init()
{
  return impl_->init();
}

unsigned long sequencer::reset(unsigned long id)
{
  return impl_->reset(id);
}

unsigned long sequencer::next()
{
  return impl_->next();
}

unsigned long sequencer::current() const
{
  return impl_->current();
}

unsigned long sequencer::update(unsigned long id)
{
  return impl_->update(id);
}

}
