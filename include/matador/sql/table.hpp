#ifndef QUERY_TABLE_HPP
#define QUERY_TABLE_HPP

#include "matador/sql/column.hpp"

#include <typeindex>
#include <string>
#include <vector>

namespace matador::sql {

struct table
{
  table(const char *name, std::string as = "") // NOLINT(*-explicit-constructor)
  : name(name), alias(std::move(as)) {}
  table(std::string name, std::string as = "") // NOLINT(*-explicit-constructor)
  : name(std::move(name)), alias(std::move(as)) {}

  table& as(const std::string &a) {
    alias = a;
    return *this;
  }

  std::string name;
  std::string alias;

  std::vector<column> columns;
};

}

#endif //QUERY_TABLE_HPP
