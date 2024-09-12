#ifndef QUERY_TABLE_HPP
#define QUERY_TABLE_HPP

#include "matador/sql/column.hpp"

#include <string>
#include <vector>

namespace matador::sql {

struct column;

struct table
{
  table() = default;
  table(const char *name); // NOLINT(*-explicit-constructor)
  table(std::string name); // NOLINT(*-explicit-constructor)
  table(const char *name, std::string as); // NOLINT(*-explicit-constructor)
  table(std::string name, std::string as); // NOLINT(*-explicit-constructor)
  table(std::string name, std::string as, const std::vector<column> &columns)
  : name(std::move(name))
  , alias(std::move(as))
  , columns(columns) {}

  table& as(const std::string &a);

  [[nodiscard]] bool operator==(const table &x) const;

  [[nodiscard]] table as(const std::string &a) const;

  [[nodiscard]] bool has_alias() const;

  std::string name;
  std::string alias;

  std::vector<column> columns;
};

table operator "" _tab(const char *name, size_t len);

}

#endif //QUERY_TABLE_HPP
