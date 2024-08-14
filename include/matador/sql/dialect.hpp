#ifndef QUERY_DIALECT_HPP
#define QUERY_DIALECT_HPP

#include "matador/sql/column.hpp"
#include "matador/sql/dialect_token.hpp"
#include "matador/sql/query_compiler.hpp"

#include "matador/utils/data_types.hpp"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace matador::sql {

class dialect final
{
public:
  /**
   * Holding enums concerning escaping identifiers
   */
  enum class escape_identifier_t : uint8_t {
    ESCAPE_BOTH_SAME,       /**< The escape quotes are the same  */
    ESCAPE_CLOSING_BRACKET  /**< The escape quotes differ; escape the closing one */
  };

  using token_to_string_map = std::unordered_map<dialect_token, std::string>;
  using data_type_to_string_map = std::unordered_map<data_type, std::string>;
  using sql_func_to_string_map = std::unordered_map<sql_function_t, std::string>;

  using next_placeholder_func = std::function<std::string(size_t)>;

public:
  [[nodiscard]] const std::string& token_at(dialect_token token) const;
  [[nodiscard]] const std::string& data_type_at(data_type type) const;

  /**
   * Prepare sql dialect identifier for execution
   * and escape quotes and quote the identifier
   * string
   *
   * @param col The identifier string to be prepared
   * @return The prepared string
   */
  [[nodiscard]] std::string prepare_identifier(const column &col) const;
  [[nodiscard]] std::string prepare_identifier_string(const std::string &col) const;

  /**
   * Prepare string literal
   *
   * @param str String literal to be prepared
   */
  [[nodiscard]] std::string prepare_literal(const std::string &str) const;

  /**
   * Wrap identifier quotes around a sql identifier keyword
   *
   * @param str Identifier to put quotes around
   */
  void quote_identifier(std::string &str) const;

  /**
   * Escape identifier quotes inside identifiers.
   *
   * @param str Identifier to be escaped
   */
  void escape_quotes_in_identifier(std::string &str) const;

  /**
   * Escape quotes in string literals
   *
   * @param str String literal to be escaped
   */
  void escape_quotes_in_literals(std::string &str) const;

  /**
   * Returns how the identifier quotes should be
   * escaped.
   *
   * @return How the identifier quotes should be escaped
   */
  [[nodiscard]] escape_identifier_t identifier_escape_type() const;

  /**
   * Generates a next placeholder string. default is
   * question mark '?'
   *
   * @return Placeholder string
   */
  [[nodiscard]] std::string next_placeholder(const std::vector<std::string> &bind_vars) const;

  /**
   * Returns the default schema name.
   *
   * @return The default schema name.
   */
  [[nodiscard]] std::string default_schema_name() const;

  /**
   * Returns the current query compiler
   * of the dialect
   *
   * @return The current query compiler
   */
  query_compiler& compiler() const;

  void compiler(std::unique_ptr<query_compiler> &&cmpl);

private:
  dialect();

private:
  friend class dialect_builder;

  next_placeholder_func placeholder_func_ = [](size_t) { return "?"; };

  std::string default_schema_name_;
  std::unique_ptr<query_compiler> compiler_;

  token_to_string_map tokens_ {
    {dialect_token::CREATE,      "CREATE"},
    {dialect_token::DROP,        "DROP"},
    {dialect_token::REMOVE,      "DELETE"},
    {dialect_token::INSERT,      "INSERT"},
    {dialect_token::TABLE,       "TABLE"},
    {dialect_token::INTO,        "INTO"},
    {dialect_token::VALUES,      "VALUES"},
    {dialect_token::UPDATE,      "UPDATE"},
    {dialect_token::SELECT,      "SELECT"},
    {dialect_token::COLUMNS,     "COLUMNS"},
    {dialect_token::COLUMN,      "COLUMN"},
    {dialect_token::FROM,        "FROM"},
    {dialect_token::JOIN,        "INNER JOIN"},
    {dialect_token::ON,          "ON"},
    {dialect_token::WHERE,       "WHERE"},
    {dialect_token::AND,         "AND"},
    {dialect_token::OR,          "OR"},
    {dialect_token::LIKE,        "LIKE"},
    {dialect_token::ORDER_BY,    "ORDER BY"},
    {dialect_token::GROUP_BY,    "GROUP BY"},
    {dialect_token::ASC,         "ASC"},
    {dialect_token::DESC,        "DESC"},
    {dialect_token::OFFSET,      "OFFSET"},
    {dialect_token::LIMIT,       "LIMIT"},
    {dialect_token::AS,          "AS"},
    {dialect_token::OFFSET,      "OFFSET"},
    {dialect_token::DISTINCT,    "DISTINCT"},
    {dialect_token::SET,         "SET"},
    {dialect_token::NOT_NULL,    "NOT NULL"},
    {dialect_token::PRIMARY_KEY, "PRIMARY KEY"},
    {dialect_token::BEGIN,       "BEGIN TRANSACTION"},
    {dialect_token::COMMIT,      "COMMIT TRANSACTION"},
    {dialect_token::ROLLBACK,    "ROLLBACK TRANSACTION"},
    {dialect_token::START_QUOTE, "\""},
    {dialect_token::END_QUOTE, "\""},
    {dialect_token::STRING_QUOTE, "'"},
    {dialect_token::BEGIN_BINARY_DATA, "X'"},
    {dialect_token::END_BINARY_DATA, "'"},
    {dialect_token::NONE, ""}
  };

  data_type_to_string_map data_types_ {
    {data_type::type_char, "TINYINT"},
    {data_type::type_short, "SMALLINT"},
    {data_type::type_int, "INTEGER"},
    {data_type::type_long, "BIGINT"},
    {data_type::type_long_long, "BIGINT"},
    {data_type::type_unsigned_char, "TINYINT"},
    {data_type::type_unsigned_short, "INTEGER"},
    {data_type::type_unsigned_int, "BIGINT"},
    {data_type::type_unsigned_long, "BIGINT"},
    {data_type::type_unsigned_long_long, "BIGINT"},
    {data_type::type_float, "FLOAT"},
    {data_type::type_double, "DOUBLE"},
    {data_type::type_bool, "BOOLEAN"},
    {data_type::type_char_pointer, "VARCHAR"},
    {data_type::type_varchar, "VARCHAR"},
    {data_type::type_text, "TEXT"},
    {data_type::type_date, "DATE"},
    {data_type::type_time, "DATETIME"},
    {data_type::type_blob, "BLOB"},
    {data_type::type_null, "NULL"},
    {data_type::type_unknown, "UNKNOWN"}
  };

  sql_func_to_string_map sql_func_map_ {
    {sql_function_t::NONE,  "NONE" },
    {sql_function_t::COUNT, "COUNT" },
    {sql_function_t::AVG,   "AVG" },
    {sql_function_t::SUM,   "SUM" },
    {sql_function_t::MIN,   "MIN" },
    {sql_function_t::MAX,   "MAX" },
  };
};

}

#endif //QUERY_DIALECT_HPP
