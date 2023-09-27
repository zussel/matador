#ifndef MATADOR_HAS_MANY_TO_MANY_ITEM_HPP
#define MATADOR_HAS_MANY_TO_MANY_ITEM_HPP

#include "matador/object/basic_has_many_to_many_item.hpp"
#include "matador/object/object_ptr.hpp"

namespace matador {

/// @cond MATADOR_DEV

template < class LeftType, class RightType >
class has_many_to_many_item : public basic_has_many_to_many_item
{
public:
  using left_value_type = LeftType;
  using right_value_type = RightType;

  has_many_to_many_item() = default;
  has_many_to_many_item(const object_ptr<LeftType> &left, const object_ptr<RightType> &right,
                        const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column)
    , left_(left)
    , right_(right)
  {}

  has_many_to_many_item(const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left_column, right_column)
  {}

  template < class Operator >
  void process(Operator &op)
  {
    matador::access::belongs_to(op, this->left_column().c_str(), left_, matador::cascade_type::NONE);
    matador::access::belongs_to(op, this->right_column().c_str(), right_, matador::cascade_type::NONE);
  }

  basic_has_many_to_many_item *clone() const override
  {
    return new has_many_to_many_item(left_column(), right_column());
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

/// @endcond

}

#endif //MATADOR_HAS_MANY_TO_MANY_ITEM_HPP
