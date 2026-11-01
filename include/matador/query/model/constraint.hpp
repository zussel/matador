#ifndef MATADOR_QUERY_CONSTRAINT_HPP
#define MATADOR_QUERY_CONSTRAINT_HPP

#include "column_constraint.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <variant>

namespace matador::query {

class column;
class table;

class constraint final {
public:
  static constraint make_column_constraint(const table &table,
                                           std::size_t column_index, column_constraint kind);
  static constraint make_column_constraint(std::string name,
                                           std::string table_name, std::string column_name,
                                           column_constraint kind);
  static constraint make_column_constraint(std::string table_name, std::string column_name,
                                           column_constraint kind);

  static constraint make_fk_constraint(
    const table& table,
    std::size_t column_index,
    const class table& ref_table
  );
  static constraint make_fk_constraint(
    const table& table,
    std::size_t column_index,
    const class table& ref_table,
    std::size_t ref_column_index
  );
  static constraint make_fk_constraint(
    std::string name,
    std::string table_name,
    std::string column_name,
    std::string ref_table_name,
    std::string ref_column_name
  );

  constraint& operator=(const constraint& other) = default;
  constraint(const constraint& other) = default;
  constraint(constraint&& other) noexcept = default;
  ~constraint() = default;

  [[nodiscard]] bool has_name() const;
  [[nodiscard]] std::string name() const;
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

  struct column_ref {
    std::string table_name;
    std::string column_name;
  };

  constraint(std::optional<std::string> name, column_ref column, column_constraint kind, std::optional<column_ref> ref_column = std::nullopt);

  void rebind_table(const class table& table);

  friend class table;

  std::optional<std::string> name_;
  column_ref column_;
  std::optional<column_ref> ref_column_;
  column_constraint kind_;
};

} // namespace matador::query

#endif // MATADOR_QUERY_CONSTRAINT_HPP
