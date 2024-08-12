#ifndef QUERY_PRODUCT_HPP
#define QUERY_PRODUCT_HPP

#include "category.hpp"
#include "supplier.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/field_attributes.hpp"

#include "matador/object/object_ptr.hpp"

#include <string>

namespace matador::test {

struct product
{
  std::string product_name;
  object_ptr<test::supplier> supplier;
  object_ptr<test::category> category;
  std::string quantity_per_unit;
  unsigned int unit_price;
  unsigned int units_in_stock;
  unsigned int units_in_order;
  unsigned int reorder_level;
  bool discontinued;

  template<class Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    using namespace matador::utils;
    field::primary_key(op, "product_name", product_name, 255);
    field::has_one(op, "supplier_id", supplier, utils::cascade_type::ALL);
    field::has_one(op, "category_id", category, utils::cascade_type::ALL);
    field::attribute(op, "quantity_per_unit", quantity_per_unit, 255);
    field::attribute(op, "unit_price", unit_price);
    field::attribute(op, "units_in_stock", units_in_stock);
    field::attribute(op, "units_in_order", units_in_order);
    field::attribute(op, "reorder_level", reorder_level);
    field::attribute(op, "discontinued", discontinued);
  }
};
}

#endif //QUERY_PRODUCT_HPP
