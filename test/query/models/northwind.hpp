#ifndef MATADOR_NORTHWIND_HPP
#define MATADOR_NORTHWIND_HPP

#include "matador/query/relations/object_ptr.hpp"
#include "matador/query/relations/collection.hpp"

#include "matador/query/mapping/access.hpp"
#include "matador/query/model/column_options.hpp"
#include "matador/query/model/foreign_key_options.hpp"
#include "matador/query/model/primary_key_options.hpp"

#include <string>

namespace matador::test {
struct order;

struct category {
  unsigned int id{};
  std::string name;

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    using namespace matador::utils;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, UniqueVarChar255);
  }
};

struct supplier {
  unsigned int id{};
  std::string name;

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    using namespace matador::utils;
    field::primary_key(op, "id", id);
    field::attribute(op, "name", name, UniqueVarChar255);
  }
};

struct product {
  std::string product_name;
  query::object_ptr<supplier> seller;
  query::object_ptr<category> type;
  std::string quantity_per_unit;
  unsigned int unit_price{};
  unsigned int units_in_stock{};
  unsigned int units_in_order{};
  unsigned int reorder_level{};
  bool discontinued{};

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    using namespace matador::utils;
    field::primary_key(op, "product_name", product_name, query::ManualVarChar511);
    field::belongs_to(op, "supplier_id", seller, CascadeAllFetchLazy);
    field::belongs_to(op, "category_id", type, CascadeAllFetchLazy);
    field::attribute(op, "quantity_per_unit", quantity_per_unit, VarChar255);
    field::attribute(op, "unit_price", unit_price);
    field::attribute(op, "units_in_stock", units_in_stock);
    field::attribute(op, "units_in_order", units_in_order);
    field::attribute(op, "reorder_level", reorder_level);
    field::attribute(op, "discontinued", discontinued);
  }
};

struct order_details {
  unsigned int order_details_id{};
  query::object_ptr<order> order_;
  query::collection<query::object_ptr<product>> products_;

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "order_details_id", order_details_id);
    field::belongs_to(op, "order_id", order_, CascadeNoneFetchLazy);
    field::has_many(op, "order_products", products_, "product_id", CascadeNoneFetchLazy);
  }
};

struct order {
  unsigned int order_id{};
  std::string order_date;
  std::string required_date;
  std::string shipped_date;
  unsigned int ship_via{};
  unsigned int freight{};
  std::string ship_name;
  std::string ship_address;
  std::string ship_city;
  std::string ship_region;
  std::string ship_postal_code;
  std::string ship_country;
  query::collection<query::object_ptr<order_details> > order_details_;

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "order_id", order_id);
    field::attribute(op, "order_date", order_date, VarChar255);
    field::attribute(op, "required_date", required_date, VarChar255);
    field::attribute(op, "shipped_date", shipped_date, VarChar255);
    field::attribute(op, "ship_via", ship_via);
    field::attribute(op, "freight", freight);
    field::attribute(op, "ship_name", ship_name, VarChar255);
    field::attribute(op, "ship_address", ship_address, VarChar255);
    field::attribute(op, "ship_city", ship_city, VarChar255);
    field::attribute(op, "ship_region", ship_region, VarChar255);
    field::attribute(op, "ship_postal_code", ship_postal_code, VarChar255);
    field::attribute(op, "ship_country", ship_country, VarChar255);
    field::has_many(op, "order_details", order_details_, "order_id", query::fetch_type::Eager);
  }
};

}
#endif // MATADOR_NORTHWIND_HPP
