#ifndef QUERY_COLUMN_HPP
#define QUERY_COLUMN_HPP

#include <functional>
#include <memory>
#include <string>

namespace matador::sql {

struct table;

enum class sql_function_t {
  NONE,
  COUNT,
  AVG,
  SUM,
  MIN,
  MAX
};

struct column
{
  column(const char *name, const std::string& as = ""); // NOLINT(*-explicit-constructor)
  explicit column(std::string name, std::string as = ""); // NOLINT(*-explicit-constructor)
  column(sql_function_t func, std::string name); // NOLINT(*-explicit-constructor)
  column(const struct table &t, std::string name, std::string as = "");
  column(const std::shared_ptr<table> &t, std::string name, std::string as = "");

  [[nodiscard]] bool equals(const column &x) const;

  column& as(std::string a);

  [[nodiscard]] bool is_function() const;
  [[nodiscard]] bool has_alias() const;

  std::shared_ptr<table> table_;
  using table_ref = std::reference_wrapper<const table>;
  std::string name;
  std::string alias;
  sql_function_t function_{sql_function_t::NONE};
};

column operator "" _col(const char *name, size_t len);

}
#endif //QUERY_COLUMN_HPP
