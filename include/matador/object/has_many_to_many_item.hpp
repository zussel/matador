#ifndef MATADOR_HAS_MANY_TO_MANY_ITEM_HPP
#define MATADOR_HAS_MANY_TO_MANY_ITEM_HPP

#include "matador/object/basic_has_many_to_many_item.hpp"
#include "matador/object/object_ptr.hpp"
#include "matador/object/has_one.hpp"

namespace matador {

template < class L, class R >
class has_many_to_many_item : public basic_has_many_to_many_item
{
public:
  has_many_to_many_item(const object_ptr<L> &left, const object_ptr<R> &right,
                        const std::string &left_column, const std::string &right_column)
    : basic_has_many_to_many_item(left.proxy_, right.proxy_, left_column, right_column)
    , left_(left)
    , right_(right)
  {}

  object_ptr<L> left() const
  {
    return left_;
  }

  object_ptr<L> right() const
  {
    return right_;
  }

private:
  has_one<L> left_;
  has_one<R> right_;
};

}

#endif //MATADOR_HAS_MANY_TO_MANY_ITEM_HPP
