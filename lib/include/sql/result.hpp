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
#include "sql/row.hpp"

#include <memory>
#include <type_traits>
#include <iterator>
#include <functional>

namespace oos {

template < class T >
class result;

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
  base_result_iterator(oos::result<T> *res, T *obj = nullptr)
  : obj_(obj)
  , result_(res)
  {}
  base_result_iterator(base_result_iterator&& x)
  : obj_(x.obj_.release())
  , result_(x.result_)
  {}

  base_result_iterator& operator=(base_result_iterator&& x)
  {
    result_ = x.result_;
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
  result<T> *result_ = nullptr;
//  oos::detail::result_impl *result_impl_ = nullptr;
};

template < class T >
class result_iterator : public base_result_iterator<T>
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

  self & operator++();
  self operator++(int);
};

template < class T >
class result
{
public:
//  typedef T*(*t_creator_func)();
  typedef std::function<T*()> t_creator_func;
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
    return std::move(++iterator(this));
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
    return p->result_rows();
  }

  void creator(const t_creator_func &creator_func)
  {
    creator_func_ = creator_func;
  }

private:
  friend class result_iterator<T>;

private:

  T* create() const
  {
    if (creator_func_) {
      return creator_func_();
    } else {
      return new T;
    }
  }
private:
  oos::detail::result_impl *p = nullptr;
  t_creator_func creator_func_;
};

template <>
class result<row>
{
public:
  typedef result_iterator<row> iterator;

  result(const result &x) = delete;
  result& operator=(const result &x) = delete;

public:
  result() {}
  result(oos::detail::result_impl *impl, const row &prototype)
    : p(impl)
    , prototype_(prototype)
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
    return std::move(++iterator(this));
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
    return p->result_rows();
  }

private:
  friend class result_iterator<row>;

private:

  row* create() const
  {
    return new row(prototype_);
  }
private:
  oos::detail::result_impl *p = nullptr;
  const row prototype_;
};

template < class T >
typename result_iterator<T>::self &result_iterator<T>::operator++()
{
  base::obj_.reset(base::result_->create());
  base::result_->p->bind(base::obj_.get());
  if (!base::result_->p->fetch(base::obj_.get())) {
    base::obj_.reset();
  }
  return *this;
}

template < class T >
typename result_iterator<T>::self result_iterator<T>::operator++(int)
{
  std::unique_ptr<T> obj(base::obj_.release());
  base::result_->p->bind(base::obj_.get());
  if (!base::result_->p->fetch(base::obj_.get())) {
    base::obj_.reset();
  }
  return self(base::result_, obj.release());
}

/// @endcond

}

#endif /* RESULT_HPP */
