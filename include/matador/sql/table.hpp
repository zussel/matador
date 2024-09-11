#ifndef QUERY_TABLE_HPP
#define QUERY_TABLE_HPP

#include "matador/sql/column.hpp"

#include <string>
#include <vector>

namespace matador::sql {

struct column;

struct table
{
  table(const char *name, std::string as = ""); // NOLINT(*-explicit-constructor)
  table(std::string name, std::string as = ""); // NOLINT(*-explicit-constructor)
  table(std::string name, std::string as, const std::vector<column> &columns)
  : name(std::move(name))
  , alias(std::move(as))
  , columns(columns) {}

  table& as(const std::string &a);

  [[nodiscard]] table as(const std::string &a) const;

  std::string name;
  std::string alias;

  std::vector<column> columns;
};

table operator "" _tab(const char *name, size_t len);

}

#endif //QUERY_TABLE_HPP
