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

#ifndef OBJECT_PTR_HPP
#define OBJECT_PTR_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include "object/object_proxy.hpp"

#include <memory>
#include <iostream>
#include <typeinfo>

namespace oos {

class object;

class OOS_API object_base_ptr {
protected:
	explicit object_base_ptr(bool is_ref);
	object_base_ptr(const object_base_ptr &x);
	object_base_ptr& operator=(const object_base_ptr &x);
	object_base_ptr(object* o, bool is_ref);
	object_base_ptr(object_proxy *op, bool is_ref);
	virtual ~object_base_ptr();

public:
	bool operator==(const object_base_ptr &x) const;
	bool operator!=(const object_base_ptr &x) const;

  virtual const char* type() const = 0;

	void reset(object *o = 0);

	bool is_loaded() const;

	long id() const;

	object* ptr() const;

	object* lookup_object() const;

  virtual bool is_reference() const;
  bool is_internal() const;

  unsigned long ref_count() const;
  unsigned long ptr_count() const;

protected:
	friend class object_atomizer;
  friend class object_creator;
  friend class object_serializer;
  friend struct object_proxy;

  template < class T > friend class object_ref;
  template < class T > friend class object_ptr;

	long id_;
  object_proxy *proxy_;
  bool is_reference_;
  bool is_internal_;
};

template < class T, bool IR >
class object_wrapper : public object_base_ptr
{
public:
	object_wrapper() : object_base_ptr(false) {}
	template < class Y >
	object_wrapper(const object_wrapper<Y, true> &x) {}
	template < class Y >
	object_wrapper(const object_wrapper<Y, false> &x) {}
	template < class Y >
	object_wrapper& operator=(const object_wrapper<Y, true> &x) { return *this; }
	template < class Y >
	object_wrapper& operator=(const object_wrapper<Y, false> &x) { return *this; }
	explicit object_wrapper(object* o) : object_base_ptr(o, false) {}

  virtual const char* type() const
  {
    return typeid(T).name();
  }

	T* operator->() const {
	  if (proxy_) {
	    return dynamic_cast<T*>(lookup_object());
	  }
	  return NULL;
	}
	T& operator*() const {
		if (proxy_) {
      return *dynamic_cast<T*>(lookup_object());
 		}
    return NULL;
	}
	T* get() const {
		if (proxy_) {
      return dynamic_cast<T*>(lookup_object());
 		}
    return NULL;
	}
  virtual bool is_reference() const
  {
    return IR;
  }
};

template < class T >
class object_ref;

template < class T >
class object_ptr : public object_base_ptr
{
public:
	object_ptr() : object_base_ptr(false) {}
	template < class Y >
	object_ptr(const object_ptr<Y> &x) {}
	template < class Y >
  object_ptr(const object_ref<Y> &x) : object_base_ptr(x.proxy_, false) {}
	template < class Y >
	object_ptr& operator=(const object_ptr<Y> &x) { return *this; }
	explicit object_ptr(object* o) : object_base_ptr(o, false) {}
  explicit object_ptr(object_proxy *proxy) : object_base_ptr(proxy, false) {}

  virtual const char* type() const
  {
    return typeid(T).name();
  }

	T* operator->() const {
	  if (proxy_) {
	    return dynamic_cast<T*>(lookup_object());
	  }
	  return NULL;
	}
	T& operator*() const {
		if (proxy_) {
      return *dynamic_cast<T*>(lookup_object());
    } else {
      return *(T*)0;
    }
	}
	T* get() const {
    if (proxy_) {
      T* t = dynamic_cast<T*>(lookup_object());
      return t;
    }
    return NULL;
  }
};

template < class T >
class object_ref : public object_base_ptr
{
public:
	object_ref() : object_base_ptr(true) {}
	//  object_ptr(const object_ptr &x) {}
	template < class Y >
	object_ref(const object_ref<Y> &x) {}
	template < class Y >
  object_ref(const object_ptr<Y> &x) : object_base_ptr(x.proxy_, true) {}
	//  object_ptr& operator=(const object_ptr &x) { return *this; }
	template < class Y >
	object_ref& operator=(const object_ref<Y> &x) { return *this; }
	explicit object_ref(object* o) : object_base_ptr(o, true) {}
  explicit object_ref(object_proxy *proxy) : object_base_ptr(proxy, true) {}

  virtual const char* type() const
  {
    return typeid(T).name();
  }

	T* operator->() const {
	  if (proxy_) {
	    return dynamic_cast<T*>(lookup_object());
	  }
	  return NULL;
	}
	T& operator*() const {
		if (proxy_) {
      return *dynamic_cast<T*>(lookup_object());
 		}
    return NULL;
	}
	T* get() const {
		if (proxy_) {
      return dynamic_cast<T*>(lookup_object());
 		}
    return NULL;
	}
};

}

#endif /* OBJECT_PTR_HPP */
