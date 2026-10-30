#ifndef MATADOR_QUERY_CONSTRAINT_HPP
#define MATADOR_QUERY_CONSTRAINT_HPP

#include "column_constraint.hpp"

#include <cstddef>
#include <string>
#include <variant>

namespace matador::query {

class column;
class table;

class constraint final {
public:
  static constraint make_column_constraint(const table& table, std::size_t column_index, column_constraint kind);
  static constraint make_column_constraint(std::string table_name, std::string column_name, column_constraint kind);

  static constraint make_fk_constraint(const table& table, std::size_t column_index, const class table& ref_table);
  static constraint make_fk_constraint(
    const table& table,
    std::size_t column_index,
    const class table& ref_table,
    std::size_t ref_column_index
  );
  static constraint make_fk_constraint(
    std::string table_name,
    std::string column_name,
    std::string ref_table_name,
    std::string ref_column_name
  );

  constraint& operator=(const constraint& other) = default;
  constraint(const constraint& other) = default;
  constraint(constraint&& other) noexcept = default;
  ~constraint() = default;

  [[nodiscard]] column_constraint kind() const;
  [[nodiscard]] std::string table_name() const;
  [[nodiscard]] std::string column_name() const;

  [[nodiscard]] bool is_index_constraint() const;
  [[nodiscard]] bool is_unique_constraint() const;
  [[nodiscard]] bool is_primary_key_constraint() const;
  [[nodiscard]] bool is_foreign_key_constraint() const;
  [[nodiscard]] bool is_identity_constraint() const;
  [[nodiscard]] bool is_default_constraint() const;
  [[nodiscard]] bool is_not_null_constraint() const;

  [[nodiscard]] bool has_ref_table() const;
  [[nodiscard]] std::string ref_table_name() const;
  [[nodiscard]] std::string ref_column_name() const;

private:

  struct indexed_column_ref {
    const class table* table{};
    std::size_t column_index{};
  };

  struct named_column_ref {
    std::string table_name;
    std::string column_name;
  };

  using column_ref = std::variant<indexed_column_ref, named_column_ref>;
  using optional_column_ref = std::variant<std::monostate, indexed_column_ref, named_column_ref>;

  static const indexed_column_ref& as_indexed_ref(const column_ref& ref);
  static const indexed_column_ref& as_indexed_ref(const optional_column_ref& ref);
  static void validate_indexed_column_ref(const indexed_column_ref& ref, column_constraint kind);
  static void validate_named_column_ref(const named_column_ref& ref);

  constraint(column_ref column, column_constraint kind, optional_column_ref ref_column = std::monostate{});

  void rebind_table(const class table& table);

  friend class table;

  column_ref column_;
  optional_column_ref ref_column_{std::monostate{}};
  column_constraint kind_;
};

} // namespace matador::query

#endif // MATADOR_QUERY_CONSTRAINT_HPP
