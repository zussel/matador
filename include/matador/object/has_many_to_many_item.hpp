#ifndef MATADOR_HAS_MANY_TO_MANY_ITEM_HPP
#define MATADOR_HAS_MANY_TO_MANY_ITEM_HPP

#include "matador/object/basic_has_many_to_many_item.hpp"
#include "matador/object/object_ptr.hpp"
#include "matador/object/has_one.hpp"

namespace matador {

template < class L, class R, typename Enable = void >
class has_many_to_many_item;

template < class L, class R >
class has_many_to_many_item<L, R, typename std::enable_if<!is_builtin<R>::value>::type> : public basic_has_many_to_many_item
{
public:
  has_many_to_many_item() {}
  has_many_to_many_item(const object_ptr<L> &left, const object_ptr<R> &right,
                        const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left.proxy_, right.proxy_, left_column, right_column)
    , left_(left)
    , right_(right)
  {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(this->left_column().c_str(), left_, matador::cascade_type::NONE);
    serializer.serialize(this->right_column().c_str(), right_, matador::cascade_type::NONE);
  }

  object_ptr<L> left() const
  {
    return left_;
  }

  object_ptr<R> right() const
  {
    return right_;
  }

private:
  has_one<L> left_;
  has_one<R> right_;
};


template < class L, class R >
class has_many_to_many_item<L, R, typename std::enable_if<is_builtin<R>::value>::type> : public basic_has_many_to_many_item
{
public:
  has_many_to_many_item() {}
  has_many_to_many_item(const object_ptr<L> &left, const R &right,
                        const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left.proxy_, nullptr, left_column, right_column)
    , left_(left)
    , right_(right)
  {}

  template < class SERIALIZER >
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(this->left_column().c_str(), left_, matador::cascade_type::NONE);
    serializer.serialize(this->right_column().c_str(), right_);
  }

  object_ptr<L> left() const
  {
    return left_;
  }

  R right() const
  {
    return right_;
  }

private:
  has_one<L> left_;
  R right_;
};

template < class T, class Enable = void >
class has_many_item_holder;

template < class T >
class has_many_item_holder<T, typename std::enable_if<!is_builtin<T>::value>::type>
{
public:
  has_many_item_holder(const object_ptr<T> &val, object_proxy *item_proxy)
    : value_(val)
    , has_many_to_many_item_poxy_(item_proxy)
  {}

  object_ptr<T> value() const
  {
    return value_;
  }
private:
  object_ptr<T> value_;
  object_proxy *has_many_to_many_item_poxy_;
};

template < class T >
class has_many_item_holder<T, typename std::enable_if<is_builtin<T>::value>::type>
{
public:
  has_many_item_holder(const T &val, object_proxy *item_proxy)
    : value_(val)
    , has_many_to_many_item_poxy_(item_proxy)
  {}

  T value() const
  {
    return value_;
  }
private:
  T value_;
  object_proxy *has_many_to_many_item_poxy_;
};

}

#endif //MATADOR_HAS_MANY_TO_MANY_ITEM_HPP
