#ifndef QUERY_ORDER_DETAILS_HPP
#define QUERY_ORDER_DETAILS_HPP

#include "product.hpp"

#include "matador/object/object_ptr.hpp"

namespace matador::test {

struct order;

struct order_details
{
  unsigned long order_details_id;
  object_ptr<order> order_;
  object_ptr<product> product_;

  template<class Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "order_details_id", order_details_id);
    field::belongs_to(op, "order_id", order_, utils::default_foreign_attributes);
    field::has_one(op, "product_id", product_, utils::default_foreign_attributes);
  }
};

}
#endif //QUERY_ORDER_DETAILS_HPP
