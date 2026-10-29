#ifndef MATADOR_COUNTRY_HPP
#define MATADOR_COUNTRY_HPP

#include "matador/query/relations/collection.hpp"
#include "matador/query/relations/object_ptr.hpp"
#include "matador/query/mapping/access.hpp"
#include "matador/query/model/foreign_key_options.hpp"
#include "matador/query/model/primary_key_options.hpp"

#include <string>

namespace matador::test {
template<const query::primary_key_options &PkAttribute>
struct capital_pk_generator;

template<const query::primary_key_options &PkAttribute>
struct country_pk_generator {
  unsigned int id{};
  std::string name;
  query::object_ptr<capital_pk_generator<PkAttribute>> capital;

  country_pk_generator() = default;
  explicit country_pk_generator(std::string name)
  : name(std::move(name)) {}
  country_pk_generator(const unsigned int id, std::string name)
  : id(id)
  , name(std::move(name)) {}

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id, PkAttribute);
    field::attribute(op, "name", name, UniqueVarChar255);
    field::has_one(op, "capital", capital, "country_id", CascadeAllFetchEager);
  }
};

template<const query::primary_key_options &PkAttribute>
struct capital_pk_generator {
  unsigned int id{};
  std::string name;
  query::object_ptr<country_pk_generator<PkAttribute>> country;

  capital_pk_generator() = default;
  capital_pk_generator(std::string name, query::object_ptr<country_pk_generator<PkAttribute>> country)
  : name(std::move(name))
  , country(std::move(country)) {}
  capital_pk_generator(const unsigned int id, std::string name, query::object_ptr<country_pk_generator<PkAttribute>> country)
  : id(id)
  , name(std::move(name))
  , country(std::move(country)) {}

  template<class Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id, PkAttribute);
    field::attribute(op, "name", name, VarChar255);
    field::belongs_to(op, "country_id", country, CascadeAllFetchLazy);
  }
};

using country = country_pk_generator<query::Manual>;
using country_identity = country_pk_generator<query::Identity>;
using country_sequence = country_pk_generator<query::Sequence>;
using country_table = country_pk_generator<query::Table>;
using capital = capital_pk_generator<query::Manual>;
using capital_identity = capital_pk_generator<query::Identity>;
using capital_sequence = capital_pk_generator<query::Sequence>;
using capital_table = capital_pk_generator<query::Table>;
}

#endif //MATADOR_COUNTRY_HPP
