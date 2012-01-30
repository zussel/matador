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

sequencer::sequencer()
  : number_(0)
{
}

sequencer::~sequencer()
{}

long sequencer::init()
{
  return number_;
}

long sequencer::next()
{
  return ++number_;
}

long sequencer::current() const
{
  return number_;
}

long sequencer::update(long id)
{
  if (id > number_) {
    number_ = id;
  }
  return number_;
}

}
