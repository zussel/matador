//
// Created by sascha on 09.05.17.
//

#ifndef MATADOR_HAS_ONE_TO_MANY_ITEM_HPP
#define MATADOR_HAS_ONE_TO_MANY_ITEM_HPP

#include "matador/utils/is_builtin.hpp"
#include "matador/utils/varchar.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/has_one.hpp"
#include "matador/object/belongs_to.hpp"
#include "matador/object/basic_has_many_to_many_item.hpp"

#include <string>

namespace matador {

/// @cond MATADOR_DEV


/**
 * Left is owner type
 * Right is value type
 *
 * @tparam L owner type
 * @tparam R value type
 * @tparam Enable
 */
template<class L, class R, typename Enable = void>
class has_one_to_many_item;

template<class L, class R>
class has_one_to_many_item<L, R, typename std::enable_if<!is_builtin<R>::value>::type>
  : public basic_has_many_to_many_item
{
public:
  typedef L left_value_type;
  typedef R right_value_type;

  has_one_to_many_item() = default;

  has_one_to_many_item(const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column)
  {}

  has_one_to_many_item(const object_ptr<L> &left, const object_ptr<R> &right,
                       const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column), left_(left), right_(right)
  {}

  template<class SERIALIZER>
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
  belongs_to<R> right_;
};


template<class L, class R>
class has_one_to_many_item<L, R, typename std::enable_if<is_builtin<R>::value && !std::is_base_of<varchar_base, R>::value>::type>
  : public basic_has_many_to_many_item
{
public:
  typedef L left_value_type;
  typedef R right_value_type;

  has_one_to_many_item() = default;

  has_one_to_many_item(const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column)
  {}

  has_one_to_many_item(const object_ptr<L> &left, const R &right,
                       const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column), left_(left), right_(right)
  {}

  template<class SERIALIZER>
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

template<class L, class R, int SIZE>
class has_one_to_many_item<L, varchar<SIZE, R>, typename std::enable_if<is_builtin<R>::value>::type>
  : public basic_has_many_to_many_item
{
public:
  typedef L left_value_type;
  typedef R right_value_type;

  has_one_to_many_item() = default;

  has_one_to_many_item(const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column)
  {}

  has_one_to_many_item(const object_ptr<L> &left, const varchar<255, R> &right,
                       const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column), left_(left), right_(right)
  {}

  has_one_to_many_item(const object_ptr<L> &left, const R &right,
                       const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column), left_(left)
  {
    right_.value = right;
  }

  template<class SERIALIZER>
  void serialize(SERIALIZER &serializer)
  {
    serializer.serialize(this->left_column().c_str(), left_, matador::cascade_type::NONE);
    serializer.serialize(this->right_column().c_str(), right_.value, SIZE);
  }

  object_ptr<L> left() const
  {
    return left_;
  }

  R right() const
  {
    return right_.value;
  }

private:
  typedef varchar<SIZE, R> varchar_type;

  has_one<L> left_;
  varchar_type right_;
};

/// @endcond

}

#endif //MATADOR_HAS_ONE_TO_MANY_ITEM_HPP
