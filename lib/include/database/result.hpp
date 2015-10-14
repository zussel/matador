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

#include "object/serializable.hpp"

#include "database/result_impl.hpp"

#include <memory>

namespace oos {

class database;

namespace detail {
class result_impl;
}

/// @cond OOS_DEV
template < class T >
class result_iterator  : public std::iterator<std::forward_iterator_tag, T>
{
public:
  typedef result_iterator<T> self; /**< Shortcut for this class. */
  typedef T value_type;            /**< Shortcut for the value type. */
  typedef value_type* pointer;     /**< Shortcut for the pointer type. */
  typedef value_type& reference;   /**< Shortcut for the reference type */

  result_iterator() {}
  result_iterator(oos::detail::result_impl *result_impl, T *obj = nullptr)
    : obj_(obj)
    , result_impl_(result_impl)
  {}
  result_iterator(result_iterator&& x)
    : obj_(x.obj_.release())
    , result_impl_(x.result_impl_)
  {}

  result_iterator& operator=(result_iterator&& x)
  {
    result_impl_ = x.result_impl_;
    obj_.reset(x.obj_.release());
    return *this;
  }

  ~result_iterator() {}

  bool operator==(const result_iterator& rhs)
  {
    return obj_ == rhs.obj_;
  }

  bool operator!=(const result_iterator& rhs)
  {
    return obj_ != rhs.obj_;
  }

  self& operator++()
  {
    obj_.reset(result_impl_->producer()->create());
    if (!result_impl_->fetch(obj_.get())) {
      obj_.reset();
    }
    return *this;
  }

  self operator++(int)
  {
    std::unique_ptr<T> obj(result_impl_->producer()->create());
    result_impl_->fetch(obj.get());
    return self(result_impl_, obj.release());
  }

  pointer operator->()
  {
    return obj_.get();
  }

  pointer operator&()
  {
    return obj_.get();
  }

  reference operator*()
  {
    return *obj_.get();
  }

  pointer get()
  {
    return obj_.get();
  }

  pointer release()
  {
    return obj_.release();
  }

private:
  std::unique_ptr<T> obj_;
  oos::detail::result_impl *result_impl_ = nullptr;
};

template < class T >
class result
{
public:
  typedef result_iterator<T> iterator;

  result(const result &x) = delete;
  result& operator=(const result &x) = delete;

public:
  result() {}
  result(oos::detail::result_impl *impl, database *db)
    : p(impl)
    , db_(db)
  {}

  ~result()
  {
    if (p) {
      delete p;
    }
  }

  result(result &&x)
  {
    std::swap(p, x.p);
  }

  result& operator=(result &&x)
  {
    if (p) {
      delete p;
      p = nullptr;
    }
    std::swap(p, x.p);
    return *this;
  }

  iterator begin()
  {
    return std::move(++iterator(p));
  }

  iterator end()
  {
    return iterator();
  }

  bool empty () const
  {
    return false;
  }

  std::size_t size () const
  {
    return 0;
  }

private:
  oos::detail::result_impl *p = nullptr;
  database *db_ = nullptr;
};

/// @endcond

}

#endif /* RESULT_HPP */
