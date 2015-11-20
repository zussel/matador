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

#ifdef _MSC_VER
  #pragma warning(disable: 4355)
#endif

#include "serializer.hpp"
#include "object/object_proxy.hpp"

#include <stack>

namespace oos {

class object_store;
class object_base_ptr;
class serializable;
class object_proxy;
class object_container;

/**
 * @cond OOS_DEV
 * @class object_inserter
 * @brief Creates objects and object_lists
 * 
 * When an serializable is inserted into the serializable store
 * subsequently other serializable must be created and
 * inserted into the serializable store.
 * This class does these tasks.
 */
class object_inserter : public generic_deserializer<object_inserter>
{
public:
  /**
   * @brief Creates an object_inserter instance.
   * 
   * An object_inserter instance ist created for a
   * given object_store. The notify flag tells the
   * object_inserter wether the observers should be
   * notified or not.
   * 
   * @param ostore The object_store.
   */
  object_inserter(object_store &ostore);
  virtual ~object_inserter();

  void insert(object_proxy *proxy);

  template < class T >
  void read_value(const char*, T&) {}

  void read_value(const char*, char*, size_t) {}
  void read_value(const char*, object_base_ptr &x);
  void read_value(const char*, object_container &x);

private:
  typedef std::set<object_proxy*> t_object_proxy_set;

  t_object_proxy_set object_proxies_;

  std::stack<object_proxy*> object_proxy_stack_;

  object_store &ostore_;
};
/// @endcond

}

#endif /* OBJECT_CREATOR_HPP */
