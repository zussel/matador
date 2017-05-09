//
// Created by sascha on 09.05.17.
//

#ifndef MATADOR_HAS_ONE_TO_MANY_ITEM_HPP
#define MATADOR_HAS_ONE_TO_MANY_ITEM_HPP

#include "matador/utils/is_builtin.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/has_one.hpp"
#include "matador/object/belongs_to.hpp"

#include <string>

namespace matador {

class basic_has_many_to_one_item
{
protected:
  basic_has_many_to_one_item()
  {}

  basic_has_many_to_one_item(const std::string &left_column, const std::string &right_column)
    : left_column_(left_column), right_column_(right_column)
  {}

public:
  std::string left_column() const { return left_column_; }
  std::string right_column() const { return right_column_; }

private:
  std::string left_column_;
  std::string right_column_;
};

template<class L, class R, typename Enable = void>
class has_many_to_one_item;

template<class L, class R>
class has_many_to_one_item<L, R, typename std::enable_if<!is_builtin<R>::value>::type>
  : public basic_has_many_to_one_item
{
public:
  has_many_to_one_item()
  {}

  has_many_to_one_item(const object_ptr<L> &left, const object_ptr<R> &right,
                       const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_one_item(left_column, right_column), left_(left), right_(right)
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
  belongs_to<L> left_;
  has_one<R> right_;
};


template<class L, class R>
class has_many_to_one_item<L, R, typename std::enable_if<is_builtin<R>::value>::type>
  : public basic_has_many_to_one_item
{
public:
  has_many_to_one_item()
  {}

  has_many_to_one_item(const object_ptr<L> &left, const R &right,
                       const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_one_item(left_column, right_column), left_(left), right_(right)
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

}

#endif //MATADOR_HAS_ONE_TO_MANY_ITEM_HPP
