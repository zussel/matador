#ifndef QUERY_QUERY_RESULT_HPP
#define QUERY_QUERY_RESULT_HPP

#include "matador/sql/query_result_impl.hpp"

#include <functional>
#include <memory>

namespace matador::sql {

class record;

template < typename Type >
class query_result;

template < typename Type >
class query_result_iterator
{
public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = Type;
  using difference_type = std::ptrdiff_t;
  using self = query_result_iterator<Type>;  /**< Shortcut for this class. */
  using pointer = value_type*;               /**< Shortcut for the pointer type. */
  using reference = value_type&;             /**< Shortcut for the reference type */

public:
  query_result_iterator() = default;
  explicit query_result_iterator(query_result<Type> *res)
  : result_(res)
  {}
  query_result_iterator(query_result<Type> *res, std::unique_ptr<Type> obj)
  : obj_(std::move(obj))
  , result_(res)
  {}
  query_result_iterator(query_result_iterator&& x) noexcept
  : obj_(std::move(x.obj_))
  , result_(x.result_)
  {}

  query_result_iterator& operator=(query_result_iterator&& x) noexcept
  {
    result_ = x.result_;
    obj_ = std::move(x.obj_);
    return *this;
  }

  ~query_result_iterator() = default;

  bool operator==(const query_result_iterator& rhs)
  {
    return obj_ == rhs.obj_;
  }

  bool operator!=(const query_result_iterator& rhs)
  {
    return obj_ != rhs.obj_;
  }

  self& operator++()
  {
    obj_.reset(result_->create());
    result_->bind(*obj_);
    if (!result_->fetch(*obj_)) {
      obj_.reset();
    }

    return *this;
  }

  self operator++(int)
  {
    const self tmp(result_, obj_);

    obj_.reset(result_->create());
    result_->bind(*obj_);
    if (!result_->fetch(*obj_)) {
      obj_.reset();
    }

    return std::move(tmp);
  }

  pointer operator->()
  {
    return obj_.get();
  }

  reference operator*()
  {
    return *obj_;
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
  std::unique_ptr<Type> obj_;
  query_result<Type> *result_{nullptr};
};

namespace detail {

template < typename Type >
Type* create_prototype(const std::vector<column_definition> &/*prototype*/)
{
  return new Type{};
}

template <>
record* create_prototype<record>(const std::vector<column_definition> &prototype);

}

template < typename Type >
class query_result
{
public:
  using iterator = query_result_iterator<Type>;
  using creator_func = std::function<Type*()>;

public:
  explicit query_result(std::unique_ptr<query_result_impl> impl)
  : impl_(std::move(impl)) {}

  // query_result(std::unique_ptr<query_result_impl> impl, std::vector<column_definition> record_prototype)
  // : record_prototype_(std::move(record_prototype))
  // , impl_(std::move(impl)) {}

  iterator begin() { return std::move(++iterator(this)); }
  iterator end() { return {}; }

private:
  friend class query_result_iterator<Type>;

  Type* create() { return detail::create_prototype<Type>(impl_->prototype()); }

  void bind(const Type &obj)
  {
    impl_->bind(obj);
  }

  bool fetch(Type &obj)
  {
    return impl_->fetch(obj);
  }

private:
  // std::vector<column_definition> record_prototype_;
  std::unique_ptr<query_result_impl> impl_;
};

}
#endif //QUERY_QUERY_RESULT_HPP
