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

#ifndef RESULT_HPP
#define RESULT_HPP

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

#include <iterator>

#ifdef WIN32
#include <memory>
#else
#include <tr1/memory>
#endif

namespace oos {

class row;
class statement;
class object_atomizable;

/// @cond OOS_DEV
class OOS_API result_impl
{
public:
  virtual ~result_impl();

  virtual bool next() = 0;

  virtual row* current() const = 0;
};

class result;

class result_iterator : public std::iterator<std::input_iterator_tag, row>
{
public:
  typedef result_iterator self;
  typedef row value_type;
  typedef value_type* pointer;
  typedef value_type& reference;

  result_iterator();
  explicit result_iterator(result *res);
  result_iterator(const result_iterator &x);
  result_iterator& operator=(const result_iterator &x);
  ~result_iterator();

  self& operator++();

  self operator++(int);

  pointer operator->() const;

  reference operator*() const;

  bool operator==(const self &x) const;

  bool operator!=(const self &x) const;

private:
  result *res_;
};

class result
{
private:
  result(const result&);
  result& operator=(const result&);

public:
  typedef unsigned long size_type;

protected:
  result();

public:
  virtual ~result();
  
  virtual void get(object_atomizable *o) = 0;
  virtual const char* column(size_type c) const = 0;
  virtual bool fetch() = 0;
  virtual size_type affected_rows() const = 0;
  virtual size_type result_rows() const = 0;
  virtual size_type fields() const = 0;

};

typedef std::tr1::shared_ptr<result> result_ptr;

/// @endcond

}

#endif /* RESULT_HPP */
