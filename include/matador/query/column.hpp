#ifndef QUERY_COLUMN_HPP
#define QUERY_COLUMN_HPP

#include "matador/utils/types.hpp"

#include "matador/query/column_options.hpp"
#include "matador/query/expression/abstract_column_expression.hpp"
#include "matador/query/query_functions.hpp"

#include <memory>
#include <string>
#include <variant>

namespace matador::query {

class table;

// ReSharper disable CppNonExplicitConvertingConstructor
class column {
public:
  column() = default;

  [[nodiscard]] static column make_plain(
    std::string name,
    std::string alias = "",
    utils::basic_type type = utils::basic_type::Unknown,
    column_options options = {}
  );

  [[nodiscard]] static column make_plain(
    const table* tab,
    std::string name,
    std::string alias = "",
    utils::basic_type type = utils::basic_type::Unknown,
    column_options options = {},
    size_t index = 0
  );

  [[nodiscard]] static column make_query_function(
    query_functions func,
    std::string name,
    std::string alias = "",
    utils::basic_type type = utils::basic_type::Unknown,
    column_options options = {}
  );

  [[nodiscard]] static column make_query_function(
    query_functions func,
    const table* tab,
    std::string name,
    std::string alias = "",
    utils::basic_type type = utils::basic_type::Unknown,
    column_options options = {}
  );

  [[nodiscard]] static column make_expression(
    const std::shared_ptr<abstract_column_expression>& expression,
    std::string alias = ""
  );

  column& operator=(const column& other) = default;
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
  [[nodiscard]] std::string name() const;

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
  [[nodiscard]] std::string canonical_name() const;

  /**
   * Returns the alias name for the column. If no alias
   * is set, an empty string is returned.
   *
   * @return Returns the alias name for the column
   */
  [[nodiscard]] const std::string& alias() const;

  /**
   * Returns the current index of the column. If
   * there is no referenced table the index is zero (0).
   *
   * @return The current index of the column.
   */
  [[nodiscard]] size_t index() const;

  /**
   * Returns the result label name for this column in a SELECT list.
   * Semantics: alias if set, otherwise the raw column name (never table-qualified).
   *
   * @return If set the alias otherwise the raw column name
   */
  [[nodiscard]] const std::string& result_name() const;

  [[nodiscard]] utils::basic_type type() const;
  /**
   * Returns constraints declared for this schema column.
   */
  [[nodiscard]] column_constraints constraints() const;

  [[nodiscard]] bool is_plain_column() const;
  [[nodiscard]] bool is_function() const;
  [[nodiscard]] bool is_expression() const;
  [[nodiscard]] bool is_nullable() const;
  [[nodiscard]] bool is_primary_key() const;
  [[nodiscard]] bool is_foreign_key() const;
  [[nodiscard]] bool is_unique() const;
  [[nodiscard]] bool is_identity() const;

  [[nodiscard]] query_functions function() const;

  [[nodiscard]] bool has_alias() const;

  /**
   * Returns the non-owning table associated with this column.
   *
   * The returned pointer must not outlive its table.
   */
  [[nodiscard]] const class table* table() const;
  void table(const class table* tab);

  // ReSharper disable once CppNonExplicitConversionOperator
  operator std::string() const; // NOLINT(*-explicit-constructor)

  [[nodiscard]] std::shared_ptr<abstract_column_expression> expression() const;

private:
  column(const class table* tab,
         std::string  name,
         std::string  alias,
         utils::basic_type type,
         query_functions func,
         const std::shared_ptr<abstract_column_expression>& expression,
         const column_options& options,
         size_t index);


  struct plain_column {
    const class table* table{nullptr};
    std::string name;
    size_t index{};
    utils::basic_type type{utils::basic_type::Unknown};
  };

  struct query_function {
    query_functions function{query_functions::None};
    plain_column column;
  };

  using column_value = std::variant<
    plain_column,
    std::shared_ptr<abstract_column_expression>,
    query_function
  >;

  static std::string build_canonical_name(const class table *tab, const std::string& name);
  [[nodiscard]] const plain_column* plain() const;
  [[nodiscard]] plain_column* plain();

private:
  friend class table;
  friend class table_generator;

  column_value value_{plain_column{}};
  std::string alias_;
  column_options options_;
};

column operator ""_col(const char *name, size_t len);

}
#endif //QUERY_COLUMN_HPP
