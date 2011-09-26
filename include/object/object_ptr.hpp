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

#ifndef object_ptr_H
#define object_ptr_H

#include "object/object_proxy.hpp"

#include <memory>
#include <iostream>
#include <typeinfo>

namespace oos {

class object;

class base_object_ptr {
protected:
	explicit base_object_ptr(bool is_ref);
	base_object_ptr(const base_object_ptr &x);
	base_object_ptr& operator=(const base_object_ptr &x);
	base_object_ptr(object* o, bool is_ref);
	base_object_ptr(object_proxy *op, bool is_ref);
	virtual ~base_object_ptr();

public:
	bool operator==(const base_object_ptr &x) const;
	bool operator!=(const base_object_ptr &x) const;

  virtual const char* type() const = 0;

	void reset(object *o = 0);

	bool is_loaded() const;

	long id() const;

	object* ptr() const;

	object* lookup_object() const;

  bool delete_object();

  bool is_reference() const;

  unsigned long ref_count() const;
  unsigned long ptr_count() const;

protected:
	friend class object_atomizer;
  template < class T > friend class object_ref;

	long id_;
  object_proxy *proxy_;
  bool is_reference_;
};

template < class T >
class object_ptr : public base_object_ptr
{
public:
	object_ptr() : base_object_ptr(false) {}
	//  object_ptr(const object_ptr &x) {}
	template < class Y >
	object_ptr(const object_ptr<Y> &x) {}
	//  object_ptr& operator=(const object_ptr &x) { return *this; }
	template < class Y >
	object_ptr& operator=(const object_ptr<Y> &x) { return *this; }
	explicit object_ptr(object* o) : base_object_ptr(o, false) {}

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
  //friend class object_ref;
};

template < class T >
class object_ref : public base_object_ptr
{
public:
	object_ref() : base_object_ptr(true) {}
	//  object_ptr(const object_ptr &x) {}
	template < class Y >
	object_ref(const object_ref<Y> &x) {}
	template < class Y >
  object_ref(const object_ptr<Y> &x) : base_object_ptr(x.proxy_, true) {}
	//  object_ptr& operator=(const object_ptr &x) { return *this; }
	template < class Y >
	object_ref& operator=(const object_ref<Y> &x) { return *this; }
	explicit object_ref(object* o) : base_object_ptr(o, true) {}

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

#endif /* object_ptr_H */
