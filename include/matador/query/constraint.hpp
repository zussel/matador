#ifndef MATADOR_QUERY_CONSTRAINT_HPP
#define MATADOR_QUERY_CONSTRAINT_HPP

#include "matador/query/column_constraint.hpp"

#include <cstddef>

namespace matador::query {

class column;
class table;

class constraint final {
public:
  static constraint make_column_constraint(const table& table, std::size_t column_index, column_constraint kind);
  static constraint make_fk_constraint(const table& table, std::size_t column_index, const class table& ref_table);

  constraint& operator=(const constraint& other) = default;
  constraint(const constraint& other) = default;
  constraint(constraint&& other) noexcept = default;
  ~constraint() = default;

  [[nodiscard]] column_constraint kind() const;
  [[nodiscard]] const class table& table() const;
  [[nodiscard]] const class column& column() const;
  [[nodiscard]] std::size_t column_index() const;

  [[nodiscard]] bool is_index_constraint() const;
  [[nodiscard]] bool is_unique_constraint() const;
  [[nodiscard]] bool is_primary_key_constraint() const;
  [[nodiscard]] bool is_foreign_key_constraint() const;
  [[nodiscard]] bool is_identity_constraint() const;
  [[nodiscard]] bool is_default_constraint() const;
  [[nodiscard]] bool is_not_null_constraint() const;

  [[nodiscard]] bool has_ref_table() const;
  [[nodiscard]] const class table& ref_table() const;
  [[nodiscard]] std::size_t ref_column_index() const;
  [[nodiscard]] const class column& ref_column() const;

private:
  constraint(const class table& table, std::size_t column_index, column_constraint kind, const class table* ref_table);

  friend class table;

  const class table* table_;
  std::size_t column_index_;
  const class table* ref_table_{nullptr};
  std::size_t ref_column_index_{0};
  column_constraint kind_;
};

} // namespace matador::query

#endif // MATADOR_QUERY_CONSTRAINT_HPP
