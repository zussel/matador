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
class object;

/**
 * @cond OOS_DEV
 * @class object_creator
 * @brief Creates objects and object_lists
 * 
 * When an object is inserted into the object store
 * subsequently other object must be created and
 * inserted into the object store.
 * This class does these tasks.
 */
class object_creator : public object_reader
{
public:
  /**
   * @brief Creates an object_creator instance.
   * 
   * An object_creator instance ist created for a
   * given object_store. The notify flag tells the
   * object_creator wether the observers should be
   * notified or not.
   * 
   * @param ostore The object_store.
   * @param notify The flag wether the observers should be informed or not.
   */
  object_creator(object_store &ostore, bool notify);

  virtual ~object_creator();

private:
  virtual void read(const char*, object_base_ptr &x);
  virtual void read(const char*, object_container &x);

private:
  std::stack<object*> object_stack_;
  object_store &ostore_;
  bool notify_;
};
/// @endcond

}

#endif /* OBJECT_CREATOR_HPP */
