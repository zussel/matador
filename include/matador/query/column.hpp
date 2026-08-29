#ifndef QUERY_COLUMN_HPP
#define QUERY_COLUMN_HPP

#include "matador/utils/types.hpp"
// #include "matador/utils/field_attributes.hpp"

#include "matador/query/expression/abstract_column_expression.hpp"
#include "matador/query/query_functions.hpp"

#include <memory>
#include <string>

namespace matador::query {

class table;

// ReSharper disable CppNonExplicitConvertingConstructor
class column {
public:
  column() = default;
  column(const char *name); // NOLINT(*-explicit-constructor)
  column(const std::string& name); // NOLINT(*-explicit-constructor)
  column(const std::string& name, const std::string& alias);
  column(query_functions func, const std::string& name);
  column(const table* tab, const std::string& name);
  column(const table* tab, const std::string& name, const std::string& alias);
  column(const table* tab, const std::string& name, utils::basic_type type);
  column(const std::shared_ptr<abstract_column_expression>& expression);

  column(const table* tab,
               std::string  name,
               std::string  alias,
               utils::basic_type type,
               query_functions func,
               const std::shared_ptr<abstract_column_expression>& expression);

  column& operator=(const column& other);
  column(const column& other) = default;
  column(column&& other) noexcept = default;
  ~column() = default;

  [[nodiscard]] bool equals(const column &x) const;

  [[nodiscard]] column as(const std::string& alias) const;

  /**
   * Returns the canonical column name.
   *
   * @return The canonical column name
   */
  [[nodiscard]] const std::string& name() const;

  /**
   * Returns the column name without prepending
   * a table name or alias.
   *
   * @return Returns the column name
   */
  [[nodiscard]] const std::string& column_name() const;

  /**
   * Returns the canonical column name which means
   * if the column is owned by a table, the table name
   * id prepended.
   *
   * @return Returns the canonical column name
   */
  [[nodiscard]] const std::string& canonical_name() const;

  /**
   * Returns the alias name for the column. If no alias
   * is set, an empty string is returned.
   *
   * @return Returns the alias name for the column
   */
  [[nodiscard]] const std::string& alias() const;

  /**
   * Returns the result label name for this column in a SELECT list.
   * Semantics: alias if set, otherwise the raw column name (never table-qualified).
   *
   * @return If set the alias otherwise the raw column name
   */
  [[nodiscard]] const std::string& result_name() const;

  [[nodiscard]] utils::basic_type type() const;

  [[nodiscard]] bool is_function() const;
  [[nodiscard]] bool is_expression() const;
  // [[nodiscard]] bool is_nullable() const;
  // [[nodiscard]] bool is_primary_key() const;
  // [[nodiscard]] bool is_foreign_key() const;
  // [[nodiscard]] bool is_unique() const;
  // [[nodiscard]] bool is_identity() const;

  [[nodiscard]] query_functions function() const;

  [[nodiscard]] bool has_alias() const;

  [[nodiscard]] const class table* table() const;
  void table(const class table* tab);

  // ReSharper disable once CppNonExplicitConversionOperator
  operator const std::string&() const; // NOLINT(*-explicit-constructor)

  [[nodiscard]] std::shared_ptr<abstract_column_expression> expression() const;

private:
  static std::string build_canonical_name(const class table *tab, const std::string& name);

private:
  friend class table;

  const class table* table_{nullptr};
  std::string column_name_;
  std::string canonical_name_;
  std::string alias_;
  utils::basic_type type_{utils::basic_type::Unknown};
  query_functions function_;
  std::shared_ptr<abstract_column_expression> expression_;
};

column operator ""_col(const char *name, size_t len);

}
#endif //QUERY_COLUMN_HPP
