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

#ifndef OBJECT_CREATOR_HPP
#define OBJECT_CREATOR_HPP

#include "object/object_atomizer.hpp"

#include <stack>

namespace oos {

class object_store;
class object_base_ptr;
class object_list_base;
class object;

class object_creator : public object_atomizer
{
public:
  object_creator(object_store &ostore, bool notify);
  virtual ~object_creator();

  virtual void read_object(const char*, object_base_ptr &x);
  virtual void read_object_list(const char*, object_list_base &x);

private:
  std::stack<object*> object_stack_;
  object_store &ostore_;
  bool notify_;
};

}

#endif /* OBJECT_CREATOR_HPP */