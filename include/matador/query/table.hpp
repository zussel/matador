#ifndef QUERY_TABLE_HPP
#define QUERY_TABLE_HPP

#include "matador/query/column.hpp"

#include <string>
#include <vector>

namespace matador::query {

// ReSharper disable CppNonExplicitConvertingConstructor
class table {
public:
  table() = default;
  table(const char *name); // NOLINT(*-explicit-constructor)
  table(const std::string& name); // NOLINT(*-explicit-constructor)
  table(const std::string& name, const std::vector<column>& columns);
  table(const table& other);
  table& operator=(const table& other);
  table(table&& other) noexcept;
  table& operator=(table&& other) noexcept;
  ~table() = default;

  [[nodiscard]] table as(const std::string &alias) const;

  [[nodiscard]] bool operator==(const table &x) const;

  [[nodiscard]] const std::string& table_name() const;
  [[nodiscard]] const std::string& name() const;
  [[nodiscard]] std::string schema_name() const;
  [[nodiscard]] const std::vector<column>& columns() const;

  [[nodiscard]] bool has_alias() const;

  // ReSharper disable once CppNonExplicitConversionOperator
  operator const std::vector<query::column>&() const; // NOLINT(*-explicit-constructor)

  const column* operator[](const std::string& column_name) const;
  static const column* column_by_name(const table &tab, const std::string& column_name);
  static const column& column_ref_by_name(const table &tab, const std::string& column_name);

  [[nodiscard]] bool has_primary_key() const;

  [[nodiscard]] const column* primary_key_column() const;

protected:
  table(std::string name, std::string  alias, const std::vector<column>& columns);

private:
  friend column;

  std::string name_;
  std::string alias_;

  std::string schema_name_;
  std::vector<column> columns_;

  int pk_column_index_{-1};
};

template<typename Type = table>
class typed_query_table : public table {
public:
  using table::table;

  // ReSharper disable once CppMemberFunctionMayBeStatic
  Type as(std::string alias) const { return Type{std::move(alias)}; }
};
}

#endif //QUERY_TABLE_HPP
