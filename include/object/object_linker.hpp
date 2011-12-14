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
class object_base_ptr;

/**
 * @class object_linker
 * @brief Links a given object to another objects parameter
 *
 * A given object is linked to concrete parameter of
 * another object identified by the given name. The
 * application can check wether the link was created
 * or not.
 */
class OOS_API object_linker : public object_atomizer
{
public:
  /**
   * Creates the object_linker
   *
   * @param elem The object where the link should be created.
   * @param parent The object to be linked to.
   * @param name The name of the parameter of the linked object.
   */
  object_linker(object *elem, const object *parent, const std::string &name);

  virtual ~object_linker();
  
  /**
   * Returns true if the link could be made.
   *
   * @return True if the link could be made.
   */
  bool success() { return linked_; }

private:
  virtual void read_object(const char *id, object_base_ptr &x);
  
private:
  object *elem_;
  const object *parent_;
  std::string name_;
  bool linked_;
};

}

#endif /* OBJECT_LINKER_HPP */
