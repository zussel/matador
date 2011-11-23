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

#ifndef OBJECT_LINKER_HPP
#define OBJECT_LINKER_HPP

#include "object/object_atomizer.hpp"

#include <string>

namespace oos {

class object;
class base_object_ptr;

class OOS_API object_linker : public object_atomizer
{
public:
  object_linker(object *elem, const base_object_ptr &o, const std::string &name);
  virtual ~object_linker();
  
  virtual void read_object(const char *id, base_object_ptr &x);
  
  bool success() { return linked_; }

private:
  object *elem_;
  const base_object_ptr &object_;
  std::string name_;
  bool linked_;
};

}

#endif /* OBJECT_LINKER_HPP */