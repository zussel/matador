#ifndef MATADOR_HAS_ONE_TO_MANY_ITEM_HPP
#define MATADOR_HAS_ONE_TO_MANY_ITEM_HPP

#include "matador/utils/is_builtin.hpp"

#include "matador/object/object_ptr.hpp"
#include "matador/object/basic_has_many_to_many_item.hpp"

#include <string>
#include <utility>

namespace matador {

/// @cond MATADOR_DEV


/**
 * Left is owner type
 * Right is foreign type
 *
 * @tparam LeftType owner type
 * @tparam RightType foreign type
 * @tparam Enable
 */
//template<class LeftType, class RightType, typename Enable = void>
//class has_one_to_many_item;

template<class LeftType, class RightType>
class has_one_to_many_item_foreign : public basic_has_many_to_many_item
{
public:
  using left_value_type = LeftType;
  using right_value_type = RightType;

  has_one_to_many_item_foreign() = default;

  has_one_to_many_item_foreign(const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column)
  {}

  has_one_to_many_item_foreign(const object_ptr<LeftType> &left, const object_ptr<RightType> &right,
                               const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column), left_(left), right_(right)
  {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::has_one(op, this->left_column().c_str(), left_, matador::cascade_type::NONE);
    matador::access::belongs_to(op, this->right_column().c_str(), right_, matador::cascade_type::NONE);
  }

  basic_has_many_to_many_item *clone() const override
  {
    return new has_one_to_many_item_foreign(left_column(), right_column());
  }

  object_ptr<LeftType> left() const
  {
    return left_;
  }

  object_ptr<RightType> right() const
  {
    return right_;
  }

private:
  object_ptr<LeftType> left_;
  object_ptr<RightType> right_;
};


template<class LeftType, class RightType>
class has_one_to_many_item_scalar : public basic_has_many_to_many_item
{
public:
  using left_value_type = LeftType;
  using right_value_type = RightType;

  has_one_to_many_item_scalar() = default;

  explicit has_one_to_many_item_scalar(const field_attributes &attr)
  : right_attributes_(attr) {}

  has_one_to_many_item_scalar(const std::string &left_column, const std::string &right_column, const field_attributes &attr)
    : basic_has_many_to_many_item(left_column, right_column)
    , right_attributes_(attr)
  {}

  has_one_to_many_item_scalar(const object_ptr<LeftType> &left, const RightType &right,
                       const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column), left_(left), right_(right)
  {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::has_one(op, this->left_column().c_str(), left_, matador::cascade_type::NONE);
    matador::access::attribute(op, this->right_column().c_str(), right_, right_attributes_);
  }

  basic_has_many_to_many_item *clone() const override
  {
    return new has_one_to_many_item_scalar(left_column(), right_column(), right_attributes_);
  }

  object_ptr<LeftType> left() const
  {
    return left_;
  }

  RightType right() const
  {
    return right_;
  }

private:
  object_ptr<LeftType> left_;
  RightType right_ = {};
  field_attributes right_attributes_;
};

/// @endcond

}

#endif //MATADOR_HAS_ONE_TO_MANY_ITEM_HPP
