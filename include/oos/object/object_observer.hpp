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

class serializer;
class object_proxy;

/**
 * @class object_observer
 * @brief Base class for serializable observer classes
 * 
 * When interessted to observe insert, update and
 * delete actions an observer class instance must be
 * registered with serializable store.
 * Use this class as base class for all observer classes.
 */
class OOS_OBJECT_API object_observer
{
public:
  virtual ~object_observer() {}
  
  /**
   * @brief Called on serializable insertion.
   * 
   * Called when an serializable is inserted
   * into the object_store.
   * 
   * @param proxy The proxy of the inserted serializable.
   */
  virtual void on_insert(object_proxy *proxy) = 0;
  
  /**
   * @brief Called on serializable update.
   * 
   * Called when an serializable is updated
   * in the object_store.
   * 
   * @param proxy The proxy of the updated serializable.
   */
  virtual void on_update(object_proxy *proxy) = 0;
  
  /**
   * @brief Called on serializable deletion.
   * 
   * Called when an serializable is deleted
   * from the object_store.
   * 
   * @param proxy The proxy of the deleted serializable.
   */
  virtual void on_delete(object_proxy *proxy) = 0;
};

}

#endif /* OBJECT_OBSERVER_HPP */
