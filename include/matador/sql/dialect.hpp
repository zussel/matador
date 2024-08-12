#ifndef QUERY_DIALECT_HPP
#define QUERY_DIALECT_HPP

#include "matador/sql/column.hpp"
#include "matador/object/data_type_traits.hpp"

#include "matador/utils/data_types.hpp"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace matador::sql {

class dialect final
{
public:
  enum class token_t : uint8_t
  {
    CREATE = 0,
    DROP,
    REMOVE,
    INSERT,
    UPDATE,
    SELECT,
    TABLE,
    VALUES,
    INSERT_VALUES,
    COLUMNS,
    COLUMN,
    FROM,
    JOIN,
    ON,
    INTO,
    WHERE,
    WHERE_CLAUSE,
    AND,
    OR,
    LIKE,
    ORDER_BY,
    GROUP_BY,
    ASC,
    DESC,
    LIMIT,
    AS,
    OFFSET,
    DISTINCT,
    SET,
    UPDATE_VALUES,
    NOT_NULL,
    PRIMARY_KEY,
    BEGIN,
    COMMIT,
    ROLLBACK,
    START_QUOTE,
    END_QUOTE,
    STRING_QUOTE,
    BEGIN_BINARY_DATA,
    END_BINARY_DATA,
    NONE
  };

  /**
   * Holding enums concerning escaping identifiers
   */
  enum class escape_identifier_t : uint8_t {
    ESCAPE_BOTH_SAME,       /**< The escape quotes are the same  */
    ESCAPE_CLOSING_BRACKET  /**< The escape quotes differ; escape the closing one */
  };

  using token_to_string_map = std::unordered_map<token_t, std::string>;
  using data_type_to_string_map = std::unordered_map<data_type, std::string>;
  using sql_func_to_string_map = std::unordered_map<sql_function_t, std::string>;

  using next_placeholder_func = std::function<std::string(size_t)>;

public:
  [[nodiscard]] const std::string& token_at(token_t token) const;
  [[nodiscard]] const std::string& data_type_at(data_type type) const;

  /**
 * Prepare sql dialect identifier for execution
 * and escape quotes and quote the identifier
 * string
 *
 * @param str The identifier string to be prepared
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
  [[nodiscard]] virtual escape_identifier_t identifier_escape_type() const;

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

private:
  dialect() = default;

private:
  friend class dialect_builder;

  next_placeholder_func placeholder_func_ = [](size_t) { return "?"; };

  std::string default_schema_name_;

  token_to_string_map tokens_ {
    {token_t::CREATE,      "CREATE"},
    {token_t::DROP,        "DROP"},
    {token_t::REMOVE,      "DELETE"},
    {token_t::INSERT,      "INSERT"},
    {token_t::TABLE,       "TABLE"},
    {token_t::INTO,        "INTO"},
    {token_t::VALUES,      "VALUES"},
    {token_t::UPDATE,      "UPDATE"},
    {token_t::SELECT,      "SELECT"},
    {token_t::COLUMNS,     "COLUMNS"},
    {token_t::COLUMN,      "COLUMN"},
    {token_t::FROM,        "FROM"},
    {token_t::JOIN,        "INNER JOIN"},
    {token_t::ON,          "ON"},
    {token_t::WHERE,       "WHERE"},
    {token_t::AND,         "AND"},
    {token_t::OR,          "OR"},
    {token_t::LIKE,        "LIKE"},
    {token_t::ORDER_BY,    "ORDER BY"},
    {token_t::GROUP_BY,    "GROUP BY"},
    {token_t::ASC,         "ASC"},
    {token_t::DESC,        "DESC"},
    {token_t::OFFSET,      "OFFSET"},
    {token_t::LIMIT,       "LIMIT"},
    {token_t::AS,          "AS"},
    {token_t::OFFSET,      "OFFSET"},
    {token_t::DISTINCT,    "DISTINCT"},
    {token_t::SET,         "SET"},
    {token_t::NOT_NULL,    "NOT NULL"},
    {token_t::PRIMARY_KEY, "PRIMARY KEY"},
    {token_t::BEGIN,       "BEGIN TRANSACTION"},
    {token_t::COMMIT,      "COMMIT TRANSACTION"},
    {token_t::ROLLBACK,    "ROLLBACK TRANSACTION"},
    {token_t::START_QUOTE, "\""},
    {token_t::END_QUOTE, "\""},
    {token_t::STRING_QUOTE, "'"},
    {token_t::BEGIN_BINARY_DATA, "X'"},
    {token_t::END_BINARY_DATA, "'"},
    {token_t::NONE, ""}
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
