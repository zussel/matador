#ifndef QUERY_TABLE_HPP
#define QUERY_TABLE_HPP

#include "matador/query/column.hpp"
#include "matador/query/constraint.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace matador::query {

// ReSharper disable CppNonExplicitConvertingConstructor
class table {
public:
  table() = default;
  explicit table(const char *name);
  explicit table(std::string name);
  // table(std::string name, std::vector<column> columns);

  static table make_plain(std::string name, std::string schema_name = "");
  static table make_primary_key_table(std::string name, std::vector<column> columns, std::string schema_name = "");
  static table make_relation_table(std::string name, std::vector<column> columns, std::size_t join_column_index, std::size_t inverse_join_column_index, std::string schema_name = "");

  table(const table& other);
  table& operator=(const table& other);
  table(table&& other) noexcept;
  table& operator=(table&& other) noexcept;
  ~table() = default;

  [[nodiscard]] table as(const std::string &alias) const;

  /**
   * Compares relation identity (schema, base name, and alias), not schema columns.
   */
  [[nodiscard]] bool operator==(const table &x) const;

  /** Returns the unqualified base relation name. */
  [[nodiscard]] const std::string& table_name() const;
  /** Returns the SQL qualifier: alias when present, otherwise schema-qualified name. */
  [[nodiscard]] std::string name() const;
  [[nodiscard]] const std::string& schema_name() const;
  [[nodiscard]] std::string qualified_name() const;
  [[nodiscard]] const std::vector<column>& columns() const;
  [[nodiscard]] const std::vector<constraint>& constraints() const;

  void update_name(const std::string& name);

  [[nodiscard]] bool has_alias() const;

  // ReSharper disable once CppNonExplicitConversionOperator
  operator const std::vector<query::column>&() const; // NOLINT(*-explicit-constructor)

  /** Finds a column by its unqualified schema name, or returns nullptr. */
  const column* operator[](const std::string& column_name) const;
  [[nodiscard]] const column* find_column(std::string_view column_name) const;
  /** Finds a column by its unqualified schema name or throws std::invalid_argument. */
  [[nodiscard]] const column& at_column(std::string_view column_name) const;
  static const column* column_by_name(const table &tab, const std::string& column_name);
  static const column& column_ref_by_name(const table &tab, const std::string& column_name);

  [[nodiscard]] bool is_primary_key_table() const;
  [[nodiscard]] bool is_relation_table() const;

  [[nodiscard]] bool has_primary_key() const;

  [[nodiscard]] const column* primary_key_column() const;
  [[nodiscard]] const column* join_column() const;
  [[nodiscard]] const column* inverse_join_column() const;

private:
  table(std::string schema_name, std::string name, std::string alias, std::vector<column> columns);

private:
  friend class table_generator;
  friend class column;

  struct primary_key_table {
    std::optional<std::size_t> pk_column_index;
  };

  struct relation_table {
    std::size_t join_column_index{};
    std::size_t inverse_join_column_index{};
  };

  using table_value = std::variant<
    primary_key_table,
    relation_table
  >;

  static void validate_schema(const std::vector<column>& columns);
  void rebind_columns();
  void rebind_constraints();
  void create_constraints();

  [[nodiscard]] const primary_key_table* primary_key_data() const;
  [[nodiscard]] primary_key_table* primary_key_data();

  [[nodiscard]] const relation_table* relation_data() const;
  [[nodiscard]] relation_table* relation_data();

  void make_primary_key_table(std::optional<std::size_t> pk_column_index);
  void make_relation_table(std::size_t join_column_index, std::size_t inverse_join_column_index);

  std::string name_;
  std::string alias_;

  std::string schema_name_;
  std::vector<column> columns_;
  std::vector<constraint> constraints_;

  table_value value_{primary_key_table{}};
};

template<typename Type = table>
class typed_query_table : public table {
public:
  using table::table;

  explicit typed_query_table(table source)
  : table(std::move(source)) {}

  [[nodiscard]] Type as(const std::string& alias) const {
    static_assert(std::is_constructible_v<Type, table>,
                  "Type must be constructible from matador::query::table");
    return Type{table::as(alias)};
  }
};
}

#endif //QUERY_TABLE_HPP
