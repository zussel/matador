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

#ifdef _MSC_VER
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

#include "object/object_atomizer.hpp"

#include "database/result_impl.hpp"

#include <memory>
#include <object/prototype_node.hpp>
#include <object/serializable.hpp>

namespace oos {

class row;
class statement;
class serializable;

/// @cond OOS_DEV

class result_iterator  : public std::iterator<std::forward_iterator_tag, serializable>
{
public:
  typedef result_iterator self;	            /**< Shortcut for this class. */
  typedef serializable value_type;            /**< Shortcut for the value type. */
  typedef value_type* pointer;                /**< Shortcut for the pointer type. */
  typedef value_type& reference;              /**< Shortcut for the reference type */

  result_iterator();
  result_iterator(const result_iterator& x);
  result_iterator& operator=(const result_iterator& x);
  ~result_iterator();

  bool operator==(const result_iterator& rhs);
  bool operator!=(const result_iterator& rhs);

  result_iterator& operator++();
  result_iterator operator++(int);

  pointer operator->();
  pointer operator&();
  reference operator*();
  pointer get();
  pointer release();

private:
  serializable *obj;
};

class OOS_API result
{
public:
  typedef result_iterator iterator;

public:
  result();
  ~result();

  result(const result &x);
  result& operator=(const result &x);

  iterator begin();
  iterator end();

  bool empty () const;
  std::size_t size () const;

private:
  friend class result_impl;

  result(result_impl *impl);

  template < class T >
  result create_result(T *impl) const;

private:
  std::shared_ptr<detail::result_impl> p;
};

/// @endcond

}

#endif /* RESULT_HPP */
