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

#ifndef OBJECT_OBSERVER_HPP
#define OBJECT_OBSERVER_HPP

namespace oos {

class object;

/**
 * @class object_observer
 * @brief Base class for object observer classes
 * 
 * When interessted to observe insert, update and
 * delete actions an observer class instance must be
 * registered with object store.
 * Use this class as base class for all observer classes.
 */
class OOS_API object_observer
{
public:
  virtual ~object_observer() {}
  
  /**
   * @brief Called on object insertion.
   * 
   * Called when an object is inserted
   * into the object_store.
   * 
   * @param o The inserted object.
   */
  virtual void on_insert(object *o) = 0;
  
  /**
   * @brief Called on object update.
   * 
   * Called when an object is updated
   * in the object_store.
   * 
   * @param o The updated object.
   */
  virtual void on_update(object *o) = 0;
  
  /**
   * @brief Called on object deletion.
   * 
   * Called when an object is deleted
   * from the object_store.
   * 
   * @param o The deleted object.
   */
  virtual void on_delete(object *o) = 0;
};

}

#endif /* OBJECT_OBSERVER_HPP */
