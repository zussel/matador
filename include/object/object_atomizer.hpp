/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY {} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OBJECTATOMIZER_HPP
#define OBJECTATOMIZER_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
#else
  #define OOS_API
#endif

#include <string>

namespace oos {

class Date;
class Time;
class object_list_base;
class base_object_ptr;

class OOS_API object_atomizer /*: public Atomizer*/
{
public:
	virtual ~object_atomizer() {}

	virtual void write_char(const char*, char) {}
	virtual void write_float(const char*, float) {}
	virtual void write_double(const char*, double) {}
	virtual void write_int(const char*, int) {}
	virtual void write_long(const char*, long) {}
	virtual void write_unsigned(const char*, unsigned) {}
	virtual void write_bool(const char*, bool) {}
	virtual void write_charptr(const char*, const char*) {}
	virtual void write_string(const char*, const std::string&) {}
	virtual void write_date(const char*, const Date&) {}
	virtual void write_time(const char*, const Time&) {}
	virtual void write_object(const char*, const base_object_ptr&) {}
	virtual void write_object_list(const char*, const object_list_base&) {}

	virtual void read_char(const char*, char&) {}
	virtual void read_float(const char*, float&) {}
	virtual void read_double(const char*, double&) {}
	virtual void read_int(const char*, int&) {}
	virtual void read_long(const char*, long&) {}
	virtual void read_unsigned(const char*, unsigned&) {}
	virtual void read_bool(const char*, bool&) {}
	virtual void read_charptr(const char*, char*&) {}
	virtual void read_string(const char*, std::string&) {}
	virtual void read_date(const char*, Date&) {}
	virtual void read_time(const char*, Time&) {}
	virtual void read_object(const char*, base_object_ptr&) {}
	virtual void read_object_list(const char*, object_list_base&) {}
};

}

#endif /* OBJECTATOMIZER_HPP */
