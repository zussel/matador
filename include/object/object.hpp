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

#ifndef OBJECT_HPP
#define OBJECT_HPP

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

#ifdef WIN32
#include <memory>
#else
#include <tr1/memory>
#endif
#include <iostream>

namespace oos {

struct object_proxy;
class object_store;
class object_atomizer;

class OOS_API object
{
	// don't allow copying
	object(const object&);
	object& operator=(const object&);
	
public:
	object();
	virtual ~object();
	
	virtual void read_from(object_atomizer *a);
	virtual void write_to(object_atomizer *a) const;

	const char* object_type() const;
	long id() const;
	void id(long oid);

  // returns the containing object store
  object_store* ostore() const;

  friend OOS_API std::ostream& operator <<(std::ostream &os, const object &o);

  std::tr1::shared_ptr<object_proxy> proxy() const { return proxy_; }

protected:
	void mark_modified();

private:
	friend class object_store;
  friend class object_linker;
  friend class object_deleter;
  friend class object_base_ptr;
  friend class object_serializer;
  friend class object_list_base;
	
	long id_;
  std::tr1::shared_ptr<object_proxy> proxy_;
};

}

#endif /* OBJECT_HPP */
