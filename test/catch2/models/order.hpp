#ifndef QUERY_ORDER_HPP
#define QUERY_ORDER_HPP

#include "order_details.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/foreign_attributes.hpp"

#include "matador/object/container.hpp"

#include <vector>

namespace matador::test {

struct order
{
  unsigned long order_id{};
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
  container<order_details> order_details_;

  template<class Operator>
  void process(Operator &op) {
    namespace field = matador::access;
    field::primary_key(op, "order_id", order_id);
    field::attribute(op, "order_date", order_date, 255);
    field::attribute(op, "required_date", required_date, 255);
    field::attribute(op, "shipped_date", shipped_date, 255);
    field::attribute(op, "ship_via", ship_via);
    field::attribute(op, "freight", freight);
    field::attribute(op, "ship_name", ship_name, 255);
    field::attribute(op, "ship_address", ship_address, 255);
    field::attribute(op, "ship_city", ship_city, 255);
    field::attribute(op, "ship_region", ship_region, 255);
    field::attribute(op, "ship_postal_code", ship_postal_code, 255);
    field::attribute(op, "ship_country", ship_country, 255);
    field::has_many(op, "order_details", order_details_, "order_id", utils::fetch_type::EAGER);
  }
};

}
#endif //QUERY_ORDER_HPP
