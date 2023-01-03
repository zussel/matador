#ifndef RESULT_HPP
#define RESULT_HPP

#include "matador/sql/result_impl.hpp"
#include "matador/sql/row.hpp"

#include <memory>
#include <type_traits>
#include <iterator>
#include <functional>

namespace matador {

template < class T >
class result;

/// @cond MATADOR_DEV
template < class T >
class base_result_iterator : public std::iterator<std::forward_iterator_tag, T>
{
public:
  typedef base_result_iterator<T> self;      /**< Shortcut for this class. */
  typedef T value_type;                      /**< Shortcut for the value type. */
  typedef std::unique_ptr<T> value_type_ptr; /**< Shortcut for the value type pointer. */
  typedef value_type* pointer;               /**< Shortcut for the pointer type. */
  typedef value_type& reference;             /**< Shortcut for the reference type */

  base_result_iterator() = default;
  explicit base_result_iterator(matador::result<T> *res, T *obj = nullptr)
  : obj_(obj)
  , result_(res)
  {}
  base_result_iterator(base_result_iterator&& x) noexcept
  : obj_(x.obj_.release())
  , result_(x.result_)
  {}

  base_result_iterator& operator=(base_result_iterator&& x) noexcept
  {
    result_ = x.result_;
    obj_.reset(x.obj_.release());
    return *this;
  }

  ~base_result_iterator() = default;

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

  value_type_ptr operator*()
  {
    return std::move(obj_);
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
  value_type_ptr obj_;
  result<T> *result_ = nullptr;
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

/*#ifdef _MSC_VER
  result_iterator() {}
  result_iterator(matador::detail::result_impl *result_impl, T *obj = nullptr)
    : base(result_impl, obj)
  {}
  result_iterator(result_iterator&& x)
    : base(x.result_impl_, x.obj_.release())
  {}
#else*/
  using base_result_iterator<T>::base_result_iterator;
//#endif

  self & operator++();
  self operator++(int);
};

/// @endcond

/**
 * @brief Represents a sql query result
 * 
 * This class is used as a iterable result
 * of a sql select query. Once such a query is
 * executed one can iterate the result and each
 * row is deserialized as the object of type T
 * 
 * @tparam T The object type of the result
 */
template < class T >
class result
{
public:
  typedef std::function<T*()> t_creator_func; /**< Shortcut to a creator function for the object type */
  typedef result_iterator<T> iterator;        /**< Shortcut to the iterator type */

  result(const result &x) = delete;
  result& operator=(const result &x) = delete;

public:
  /**
   * Empty result
   */
  result() = default;
  
  /**
   * Creates a result initialized
   * by the given result implementation
   * 
   * @param impl Result implementation
   */
  explicit result(matador::detail::result_impl *impl)
    : p(impl)
  {}

  ~result() = default;

  /**
   * Copy moves a result from given
   * result
   * 
   * @param x The result ro move
   */
  result(result &&x) noexcept
  {
    std::swap(p, x.p);
  }

  /**
   * Assignment move constructor for given
   * result
   * 
   * @param x The result ro move
   * @return Reference to this
   */
  result& operator=(result &&x)
  {
    p = std::move(x.p);
    return *this;
  }

  /**
   * Returns the first object of the
   * result set
   * 
   * @return Iterator to the first object
   */
  iterator begin()
  {
    return std::move(++iterator(this));
  }

  /**
   * Returns the last element of the
   * result set
   * 
   * @return Iterator to the last element
   */
  iterator end()
  {
    return iterator();
  }

  /**
   * Returns true if the result set
   * is empty
   * 
   * @return True on empty result set
   */
  bool empty () const
  {
    return p->result_rows() == 0;
  }

  /**
   * Returns the size of the result set
   * 
   * @return Size of the result set
   */
  std::size_t size () const
  {
    return p->result_rows();
  }

  /**
   * Sets a creator function to
   * create a custom object
   * 
   * @param creator_func The custom creator function
   */
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
  std::unique_ptr<matador::detail::result_impl> p;
  t_creator_func creator_func_;
};

/**
 * @brief Represents a sql query result
 * 
 * This class is used as a iterable result
 * of a sql select query. Once such a query is
 * executed one can iterate the result and each
 * row is deserialized as an anonymous row
 */
template <>
class result<row>
{
public:
  typedef result_iterator<row> iterator; /**< Shortcut to the iterator type */

  result(const result &x) = delete;
  result& operator=(const result &x) = delete;

public:
  /**
   * Empty result
   */
  result() : prototype_(row()) {}

  /**
   * Creates a result initialized
   * by the given result implementation and
   * the given row prototype containing
   * all expected columns.
   * 
   * @param impl Result implementation
   * @param prototype The row prototype
   */
  result(matador::detail::result_impl *impl, const row &prototype)
    : p(impl)
    , prototype_(prototype)
  {}

  ~result()
  {
    delete p;
  }

  /**
   * Copy moves a result from given
   * result
   * 
   * @param x The result ro move
   */
  result(result &&x) noexcept
    : prototype_(x.prototype_)
  {
    std::swap(p, x.p);
  }

  /**
   * Assignment move constructor for given
   * result
   * 
   * @param x The result ro move
   * @return Reference to this
   */
  result& operator=(result &&x) noexcept
  {
    if (p) {
      delete p;
      p = nullptr;
    }
    std::swap(p, x.p);
    return *this;
  }

  /**
   * Returns the first row of the
   * result set
   * 
   * @return Iterator to the first row
   */
  iterator begin()
  {
    return std::move(++iterator(this));
  }

  /**
   * Returns the last element of the
   * result set
   * 
   * @return Iterator to the last element
   */
  iterator end()
  {
    return iterator();
  }

  /**
   * Returns true if the result set
   * is empty
   * 
   * @return True on empty result set
   */
  bool empty () const
  {
    return false;
  }

  /**
   * Returns the size of the result set
   * 
   * @return Size of the result set
   */
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
  matador::detail::result_impl *p = nullptr;
  const row prototype_;
};

/// @cond MATADOR_DEV
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
