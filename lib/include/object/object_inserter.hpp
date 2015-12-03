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

#include "object/object_proxy.hpp"
#include "object/object_ptr.hpp"

#include <stack>

namespace oos {

class object_store;
class object_base_ptr;
class serializable;
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
class object_inserter
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
  ~object_inserter();

  template < class T >
  void insert(object_proxy *proxy, T *o)
  {
    object_proxies_.clear();
    while (!object_proxy_stack_.empty()) {
      object_proxy_stack_.pop();
    }

    object_proxies_.insert(proxy);
    object_proxy_stack_.push(proxy);
    if (proxy->obj()) {
      oos::access::serialize(*this, *o);
    }
  }

  template < class T >
  void serialize(const char*, T&) {}

  void serialize(const char*, char*, size_t) {}

  template < class T, bool TYPE >
  void serialize(const char*, object_holder<T, TYPE> &x)
  {
    // mark serializable pointer as internal
    x.is_internal_ = true;

    if (!x.proxy_) {
      return;
    }

    if (x.is_reference()) {
      x.proxy_->link_ref();
    } else if (x.ptr() && x.id()){
      x.proxy_->link_ptr();
    }
    if (x.ptr()) {
      bool new_object = object_proxies_.insert(x.proxy_).second;
      if (x.id()) {
        // do the pointer count
        if (new_object) {
          object_proxy_stack_.push(x.proxy_);
          oos::access::deserialize(*this, *x.get());
          object_proxy_stack_.pop();
        }
      } else if (new_object){
        // new object
        insert_into_store(x.proxy_);
      }
    }
  }

  void serialize(const char*, object_container &x);
  
private:
  void insert_into_store(object_proxy *proxy);
  
private:
  typedef std::set<object_proxy*> t_object_proxy_set;

  t_object_proxy_set object_proxies_;

  std::stack<object_proxy*> object_proxy_stack_;

  object_store &ostore_;
};
/// @endcond

}

#endif /* OBJECT_CREATOR_HPP */
