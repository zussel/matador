#ifndef QUERY_DIALECT_HPP
#define QUERY_DIALECT_HPP

#include "matador/query/dialect_token.hpp"
#include "matador/query/query_functions.hpp"

#include "matador/utils/types.hpp"
#include "matador/utils/string.hpp"

#include <array>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace matador::query {
class dialect final {
public:
  /**
   * Holding enums concerning escaping identifiers
   */
  enum class escape_identifier_t : uint8_t {
    EscapeBothSame,       /**< The escape quotes are the same  */
    EscapeClosingBracket  /**< The escape quotes differ; escape the closing one */
  };

  using token_to_string_map = std::unordered_map<dialect_token, std::string>;
  using data_type_to_string_map = std::unordered_map<utils::basic_type, std::string>;
  using sql_func_to_string_map = std::unordered_map<query_functions, std::string>;

  using next_placeholder_func = std::function<std::string(size_t)>;
  using to_escaped_string_func = std::function<std::string(const utils::blob_type_t &)>;

public:
  [[nodiscard]] const std::string& token_at(dialect_token token) const;
  [[nodiscard]] const std::string& data_type_at(utils::basic_type type) const;
  [[nodiscard]] const std::string& sql_function_at(query_functions func) const;

  [[nodiscard]] const std::string& to_string(bool val) const;

  void bool_strings(const std::string &true_string, const std::string &false_string);

  /**
   * Returns how the identifier quotes should be
   * escaped.
   *
   * @return How the identifier quotes should be escaped
   */
  [[nodiscard]] escape_identifier_t identifier_escape_type() const;

  /**
   * Sets the identifier escape type. Possibilities are
   * opening and closing escape characters are the same
   * (ESCAPE_BOTH_SAME) or using a special closing
   * escape character (ESCAPE_CLOSING_BRACKET)
   *
   * @param escape_identifier Identifier escape type
   */
  void identifier_escape_type(escape_identifier_t escape_identifier);

  /**
   * Generates a next placeholder string. default is
   * question mark '?'
   *
   * @return Placeholder string
   */
  [[nodiscard]] std::string next_placeholder(const std::vector<std::string> &bind_vars) const;

  [[nodiscard]] std::string to_escaped_string(const utils::blob_type_t &value) const;

  /**
   * Returns the default schema name.
   *
   * @return The default schema name.
   */
  [[nodiscard]] std::string default_schema_name() const;

  [[nodiscard]] const std::string& add_constraint() const;
  [[nodiscard]] const std::string& alter() const;
  [[nodiscard]] const std::string& and_() const;
  [[nodiscard]] const std::string& as() const;
  [[nodiscard]] const std::string& asc() const;
  [[nodiscard]] const std::string& begin() const;
  [[nodiscard]] const std::string& asterisk() const;
  [[nodiscard]] const std::string& begin_binary_data() const;
  [[nodiscard]] const std::string& begin_string_data() const;
  [[nodiscard]] const std::string& between() const;
  [[nodiscard]] const std::string& column() const;
  [[nodiscard]] const std::string& columns() const;
  [[nodiscard]] const std::string& commit() const;
  [[nodiscard]] const std::string& constraint() const;
  [[nodiscard]] const std::string& create() const;
  [[nodiscard]] const std::string& currval() const;
  [[nodiscard]] const std::string& desc() const;
  [[nodiscard]] const std::string& distinct() const;
  [[nodiscard]] const std::string& drop() const;
  [[nodiscard]] const std::string& drop_constraint() const;
  [[nodiscard]] const std::string& end_binary_data() const;
  [[nodiscard]] const std::string& end_quote() const;
  [[nodiscard]] const std::string& end_string_data() const;
  [[nodiscard]] const std::string& exists() const;
  [[nodiscard]] const std::string& foreign_key() const;
  [[nodiscard]] const std::string& from() const;
  [[nodiscard]] const std::string& group_by() const;
  [[nodiscard]] const std::string& identity() const;
  [[nodiscard]] const std::string& in() const;
  [[nodiscard]] const std::string& insert() const;
  [[nodiscard]] const std::string& into() const;
  [[nodiscard]] const std::string& is_null() const;
  [[nodiscard]] const std::string& is_not_null() const;
  [[nodiscard]] const std::string& join() const;
  [[nodiscard]] const std::string& like() const;
  [[nodiscard]] const std::string& limit() const;
  [[nodiscard]] const std::string& nextval() const;
  [[nodiscard]] const std::string& not_() const;
  [[nodiscard]] const std::string& not_null() const;
  [[nodiscard]] const std::string& offset() const;
  [[nodiscard]] const std::string& on() const;
  [[nodiscard]] const std::string& or_() const;
  [[nodiscard]] const std::string& order_by() const;
  [[nodiscard]] const std::string& primary_key() const;
  [[nodiscard]] const std::string& references() const;
  [[nodiscard]] const std::string& remove() const;
  [[nodiscard]] const std::string& returning() const;
  [[nodiscard]] const std::string& rollback() const;
  [[nodiscard]] const std::string& schema() const;
  [[nodiscard]] const std::string& select() const;
  [[nodiscard]] const std::string& set() const;
  [[nodiscard]] const std::string& sequence() const;
  [[nodiscard]] const std::string& start_quote() const;
  [[nodiscard]] const std::string& string_quote() const;
  [[nodiscard]] const std::string& table() const;
  [[nodiscard]] const std::string& unique() const;
  [[nodiscard]] const std::string& update() const;
  [[nodiscard]] const std::string& values() const;
  [[nodiscard]] const std::string& where() const;

private:
  dialect() = default;

private:
  friend class dialect_builder;

  next_placeholder_func placeholder_func_ = [](size_t) { return "?"; };
  to_escaped_string_func to_escaped_string_func_ = [](const utils::blob_type_t &val) { return utils::to_string(val); };

  escape_identifier_t identifier_escape_type_ = escape_identifier_t::EscapeBothSame;

  std::string default_schema_name_;


  token_to_string_map tokens_ {
    {dialect_token::AddConstraint,  "ADD CONSTRAINT"},
    {dialect_token::Alter,       "ALTER"},
    {dialect_token::And,         "AND"},
    {dialect_token::As,          "AS"},
    {dialect_token::Asc,         "ASC"},
    {dialect_token::Asterisk,    "*"},
    {dialect_token::Begin,       "BEGIN TRANSACTION"},
    {dialect_token::BeginBinaryData, "X'"},
    {dialect_token::BeginStringData, "'"},
    {dialect_token::Between,     "BETWEEN"},
    {dialect_token::Column,      "COLUMN"},
    {dialect_token::Columns,     "COLUMNS"},
    {dialect_token::Commit,      "COMMIT TRANSACTION"},
    {dialect_token::Constraint,  "CONSTRAINT"},
    {dialect_token::Create,      "CREATE"},
    {dialect_token::CurrVal,     "CURRVAL"},
    {dialect_token::Desc,        "DESC"},
    {dialect_token::Distinct,    "DISTINCT"},
    {dialect_token::Drop,        "DROP"},
    {dialect_token::DropConstraint, "DROP CONSTRAINT"},
    {dialect_token::EndBinaryData, "'"},
    {dialect_token::EndQuote, "\""},
    {dialect_token::EndStringData, "'"},
    {dialect_token::Exists,      "EXISTS"},
    {dialect_token::ForeignKey,  "FOREIGN KEY"},
    {dialect_token::From,        "FROM"},
    {dialect_token::GroupBy,     "GROUP BY"},
    {dialect_token::Identity,    "AUTO INCREMENT"},
    {dialect_token::In,          "IN"},
    {dialect_token::Insert,      "INSERT"},
    {dialect_token::Into,        "INTO"},
    {dialect_token::IsNull,      "IS NULL"},
    {dialect_token::IsNotNull,   "IS NOT NULL"},
    {dialect_token::Join,        "LEFT JOIN"},
    {dialect_token::Like,        "LIKE"},
    {dialect_token::Limit,       "LIMIT"},
    {dialect_token::NextVal,     "NEXTVAL"},
    {dialect_token::None,        ""},
    {dialect_token::Not,         "NOT"},
    {dialect_token::NotNull,     "NOT NULL"},
    {dialect_token::Offset,      "OFFSET"},
    {dialect_token::On,          "ON"},
    {dialect_token::Or,          "OR"},
    {dialect_token::OrderBy,     "ORDER BY"},
    {dialect_token::PrimaryKey,  "PRIMARY KEY"},
    {dialect_token::References,  "REFERENCES"},
    {dialect_token::Remove,      "DELETE"},
    {dialect_token::Returning,   "RETURNING"},
    {dialect_token::Rollback,    "ROLLBACK TRANSACTION"},
    {dialect_token::Schema,      "SCHEMA"},
    {dialect_token::Select,      "SELECT"},
    {dialect_token::Set,         "SET"},
    {dialect_token::Sequence,    "SEQUENCE"},
    {dialect_token::StartQuote,  "\""},
    {dialect_token::StringQuote, "'"},
    {dialect_token::Table,       "TABLE"},
    {dialect_token::Unique,      "UNIQUE"},
    {dialect_token::Update,      "UPDATE"},
    {dialect_token::Values,      "VALUES"},
    {dialect_token::Where,       "WHERE"}
  };
  data_type_to_string_map data_types_ {
    {utils::basic_type::Int8, "TINYINT"},
    {utils::basic_type::Int16, "SMALLINT"},
    {utils::basic_type::Int32, "INTEGER"},
    {utils::basic_type::Int64, "BIGINT"},
    {utils::basic_type::UInt8, "TINYINT"},
    {utils::basic_type::UInt16, "INTEGER"},
    {utils::basic_type::UInt32, "BIGINT"},
    {utils::basic_type::UInt64, "BIGINT"},
    {utils::basic_type::Float, "FLOAT"},
    {utils::basic_type::Double, "DOUBLE"},
    {utils::basic_type::Boolean, "BOOLEAN"},
    {utils::basic_type::Varchar, "VARCHAR"},
    {utils::basic_type::Text, "TEXT"},
    {utils::basic_type::Date, "DATE"},
    {utils::basic_type::Time, "TIME"},
    {utils::basic_type::DateTime, "DATETIME"},
    {utils::basic_type::Blob, "BLOB"},
    {utils::basic_type::Null, "NULL"},
    // {utils::basic_type::Unknown, "INTEGER"}
  };

  sql_func_to_string_map sql_func_map_ {
    {query_functions::None,  "NONE" },
    {query_functions::Count, "COUNT" },
    {query_functions::Avg,   "AVG" },
    {query_functions::Sum,   "SUM" },
    {query_functions::Min,   "MIN" },
    {query_functions::Max,   "MAX" },
  };

  std::array<std::string, 2> bool_strings_ {
    "0", "1"
  };
};

}

#endif //QUERY_DIALECT_HPP
