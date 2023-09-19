#ifndef MATADOR_HAS_MANY_TO_MANY_ITEM_HPP
#define MATADOR_HAS_MANY_TO_MANY_ITEM_HPP

#include "matador/object/basic_has_many_to_many_item.hpp"
#include "matador/object/object_ptr.hpp"

namespace matador {

/// @cond MATADOR_DEV

template < class L, class R >
class has_many_to_many_item : public basic_has_many_to_many_item
{
public:
  typedef L left_value_type;
  typedef R right_value_type;

  has_many_to_many_item() = default;
  has_many_to_many_item(const object_ptr<L> &left, const object_ptr<R> &right,
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

  object_ptr<L> left() const
  {
    return left_;
  }

  object_ptr<R> right() const
  {
    return right_;
  }

private:
  object_ptr<L> left_;
  object_ptr<R> right_;
};

/// @endcond

}

#endif //MATADOR_HAS_MANY_TO_MANY_ITEM_HPP
