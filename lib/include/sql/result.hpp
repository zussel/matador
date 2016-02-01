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

#include "sql/result_impl.hpp"

#include <memory>
#include <type_traits>
#include <iterator>

namespace oos {

class database;

/// @cond OOS_DEV
template < class T >
class base_result_iterator : public std::iterator<std::forward_iterator_tag, T>
{
public:
  typedef base_result_iterator<T> self; /**< Shortcut for this class. */
  typedef T value_type;            /**< Shortcut for the value type. */
  typedef value_type* pointer;     /**< Shortcut for the pointer type. */
  typedef value_type& reference;   /**< Shortcut for the reference type */

  base_result_iterator() {}
  base_result_iterator(oos::detail::result_impl *result_impl, T *obj = nullptr)
  : obj_(obj)
  , result_impl_(result_impl)
  {}
  base_result_iterator(base_result_iterator&& x)
  : obj_(x.obj_.release())
  , result_impl_(x.result_impl_)
  {}

  base_result_iterator& operator=(base_result_iterator&& x)
  {
    result_impl_ = x.result_impl_;
    obj_.reset(x.obj_.release());
    return *this;
  }

  ~base_result_iterator() {}

  bool operator==(const base_result_iterator& rhs)
  {
    return obj_ == rhs.obj_;
  }

  bool operator!=(const base_result_iterator& rhs)
  {
    return obj_ != rhs.obj_;
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

protected:
  std::unique_ptr<T> obj_;
  oos::detail::result_impl *result_impl_ = nullptr;
};

template < class T, class Enable = void >
class result_iterator {};

template < class T >
class result_iterator<T,typename std::enable_if< std::is_same<T, oos::serializer>::value >::type> : public base_result_iterator<T>
{
public:
  typedef base_result_iterator<T> base;
  typedef result_iterator<T> self; /**< Shortcut for this class. */
  typedef T value_type;            /**< Shortcut for the value type. */
  typedef value_type* pointer;     /**< Shortcut for the pointer type. */
  typedef value_type& reference;   /**< Shortcut for the reference type */

#ifdef _MSC_VER
  result_iterator() {}
  result_iterator(oos::detail::result_impl *result_impl, T *obj = nullptr)
    : base(result_impl, obj)
  {}
  result_iterator(result_iterator&& x)
    : base(x.result_impl_, x.obj_.release())
  {}
#else
  using base_result_iterator<T>::base_result_iterator;
#endif

  self& operator++()
  {
    serializer *obj = base::result_impl_->producer()->create();
    base::obj_.reset(obj);
    if (!base::result_impl_->fetch(base::obj_.get())) {
      base::obj_.reset();
    }
    return *this;
  }

  self operator++(int)
  {
    std::unique_ptr<T> obj(static_cast<T>(base::result_impl_->producer()->create()));
    base::result_impl_->fetch(obj.get());
    return self(base::result_impl_, obj.release());
  }
};

template < class T >
class result_iterator<T,typename std::enable_if< !std::is_same<T, oos::serializer>::value >::type> : public base_result_iterator<T>
{
public:
  typedef base_result_iterator<T> base;
  typedef result_iterator<T> self; /**< Shortcut for this class. */
  typedef T value_type;            /**< Shortcut for the value type. */
  typedef value_type* pointer;     /**< Shortcut for the pointer type. */
  typedef value_type& reference;   /**< Shortcut for the reference type */

#ifdef _MSC_VER
  result_iterator() {}
  result_iterator(oos::detail::result_impl *result_impl, T *obj = nullptr)
    : base(result_impl, obj)
  {}
  result_iterator(result_iterator&& x)
    : base(x.result_impl_, x.obj_.release())
  {}
#else
  using base_result_iterator<T>::base_result_iterator;
#endif

  self& operator++()
  {
    base::obj_.reset(new T);
    if (!base::result_impl_->fetch(base::obj_.get())) {
      base::obj_.reset();
    }
    return *this;
  }

  self operator++(int)
  {
    std::unique_ptr<T> obj(new T);
    base::result_impl_->bind(obj.get());
    base::result_impl_->fetch(obj.get());
    return self(base::result_impl_, obj.release());
  }
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
  result(oos::detail::result_impl *impl)
    : p(impl)
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
};

/// @endcond

}

#endif /* RESULT_HPP */
