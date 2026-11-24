#ifndef MATADOR_SHIPMENT_HPP
#define MATADOR_SHIPMENT_HPP

#include "matador/query/mapping/access.hpp"
#include "matador/query/model/foreign_key_options.hpp"

#include "matador/query/relations/object_ptr.hpp"
#include "matador/query/relations/collection.hpp"

#include <string>

namespace matador::test {
struct package;

struct shipment {
  shipment() = default;
  shipment(const long id, std::string tracking_number)
  : id(id), tracking_number(std::move(tracking_number)) {}

  int64_t id{};
  std::string tracking_number;
  query::collection<query::object_ptr<package> > packages{};

  template<typename Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "tracking_number", tracking_number, UniqueVarChar255);
    field::has_many(op, "packages", packages, "shipment_id", CascadeAllFetchEager);
  }
};

struct package {
  package() = default;
  package(const long id, const double weight, query::object_ptr<shipment> del)
  : id(id), weight(weight), delivery(std::move(del)){}

  int64_t id{};
  double weight{};
  query::object_ptr<shipment> delivery{};

  template<typename Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "weight", weight);
    field::belongs_to(op, "shipment_id", delivery, CascadeAllFetchLazy);
  }
};
}

#endif //MATADOR_SHIPMENT_HPP
