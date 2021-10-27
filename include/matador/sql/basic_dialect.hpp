//
// Created by sascha on 2/25/16.
//

#ifndef OOS_BASIC_DIALECT_HPP
#define OOS_BASIC_DIALECT_HPP

#ifdef _MSC_VER
#ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "matador/sql/types.hpp"
#include "matador/sql/token.hpp"
#include "matador/sql/token_list.hpp"
#include "matador/sql/token_visitor.hpp"

#include <unordered_map>
#include <memory>
#include <string>
#include <list>
#include <stack>

namespace matador {

class basic_dialect;
class sql;

namespace detail {

class basic_dialect_compiler;
class basic_dialect_linker;
class basic_query;

/// @cond MATADOR_DEV

struct OOS_SQL_API build_info {
  build_info(const sql &s, basic_dialect *d);

  build_info(const build_info&) = default;
  build_info(build_info&&) = default;
  build_info& operator=(const build_info&) = default;
  build_info& operator=(build_info&&) = default;

  basic_dialect *dialect;
  token_list_t tokens_;
  token_list_t::iterator current;
  std::string result;
};

/// @endcond

}

/**
 * Struct holding sql dialect traits
 */
struct dialect_traits {
  /**
   * Holding enums concerning escaping identifiers
   */
  enum identifier {
    ESCAPE_BOTH_SAME,       /**< The escape quotes are the same  */
    ESCAPE_CLOSING_BRACKET  /**< The escape quotes differ; escape the closing one */
  };
};

/**
 * @brief Abstract base class for a concrete sql dialect.
 *
 * This class acts as an abstract base class for a
 * concrete sql dialect.
 *
 * It is used as the statement builder to build sql
 * statement strings from a sql object.
 *
 * The statement can be build for direct execution or
 * for a prepared statement.
 *
 * Internally it held a map of all sql dialect tokens
 * which could eventually overwritten by the concrete
 * dialect.
 */
class OOS_SQL_API basic_dialect
{
public:
  /**
   * @brief Enum representing the compile type
   */
  enum t_compile_type {
    PREPARED, /**< Compile type for prepared statements */
    DIRECT    /**< Compile type for direct execute statements */
  };

protected:
  /**
   * @brief Creates a new dialect
   *
   * Creates a new dialect consisting of a dialect compiler
   * and dialect linker class.
   * Their are default implementations of these classes, but
   * they could be overwritten by the concrete dialect impementation
   *
   * @param compiler The dialect compiler object
   * @param linker  The dialect linker object
   */
  explicit basic_dialect(detail::basic_dialect_compiler *compiler, detail::basic_dialect_linker *linker);

public:
  virtual ~basic_dialect();

  /**
   * @brief Build a sql statement for direct execution
   *
   * @param s The sql object to be compiled and linked
   * @return The sql string for direct execution
   */
  std::string direct(const sql &s);

  /**
   * @brief Build a sql statement as a prepared statement
   *
   * @param s The sql object to be compiled and linked
   * @return The sql string as a prepared statement
   */
  std::tuple<std::string, std::vector<std::string>, std::vector<std::string>> prepare(const sql &s);
//  std::string prepare(const sql &s);

  /**
   * Prepare sql dialect identifier for execution
   * and escape quotes and quote the identifier
   * string
   *
   * @param str The identifier string to be prepared
   * @return The prepared string
   */
  std::string prepare_identifier(const std::string &str);

  /**
   * Prepare string literal
   *
   * @param str String literal to be prepared
   */
  std::string prepare_literal(const std::string &str) const;

  /**
   * Wrap identifier quotes around a sql identifier keyword
   *
   * @param str Identifier to put quotes around
   */
  void quote_identifier(std::string &str);

  /**
   * Escape identifier quotes inside identifiers.
   *
   * @param str Identifier to be escaped
   */
  void escape_quotes_in_identifier(std::string &str);

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
  virtual dialect_traits::identifier identifier_escape_type() const = 0;

  /**
   * Generates a next placeholder string. default is
   * questionmark '?'
   *
   * @return Placeholder string
   */
  virtual std::string next_placeholder() const;

  /**
   * Return the identifier opening quote
   *
   * @return Identifier opening quote.
   */
  char identifier_opening_quote() const;

  /**
   * Return the identifier closing quote
   *
   * @return Identifier closing quote.
   */
  char identifier_closing_quote() const;

  /**
   * Return database specific sql token
   *
   * @param tok Requested token
   * @return Database specific sql token
   */
  std::string token_at(detail::token::t_token tok) const;

protected:
  /// @cond MATADOR_DEV

  friend class detail::basic_dialect_compiler;
  friend class detail::basic_dialect_linker;
  template < class L, class R, class E > friend class condition;

  virtual const char* to_database_type_string(data_type type) const = 0;
  t_compile_type compile_type() const;

  bool is_preparing() const;

  std::string build(const sql &s, t_compile_type compile_type);
  std::string continue_build(const sql &s, t_compile_type compile_type);

  void replace_token(detail::token::t_token tkn, const std::string &value);

  void append_to_result(const std::string &part);

  void push(const sql &s);
  void pop();
  detail::build_info& top();

  void add_host_var(const std::string &host_var);
  void add_column(const std::string &column);

  const std::vector<std::string>& host_vars() const;
  const std::vector<std::string>& columns() const;

  ///  @endcond

private:
  void compile();
  void link();

  t_compile_type compile_type_ = DIRECT;

  detail::basic_dialect_compiler* compiler_;
  detail::basic_dialect_linker* linker_;

  std::stack<detail::build_info> build_info_stack_;

  std::vector<std::string> host_vars_;
  std::vector<std::string> columns_;

  typedef std::unordered_map<detail::token::t_token, std::string, std::hash<int>> t_token_map;
  t_token_map tokens {
    {detail::token::CREATE_TABLE, "CREATE TABLE"},
    {detail::token::DROP, "DROP TABLE"},
    {detail::token::REMOVE, "DELETE"},
    {detail::token::INSERT, "INSERT INTO"},
    {detail::token::VALUES, "VALUES"},
    {detail::token::UPDATE, "UPDATE"},
    {detail::token::SELECT, "SELECT"},
    {detail::token::COLUMNS, "COLUMNS"},
    {detail::token::COLUMN, "COLUMN"},
    {detail::token::FROM, "FROM"},
    {detail::token::WHERE, "WHERE"},
    {detail::token::AND, "AND"},
    {detail::token::OR, "OR"},
    {detail::token::LIKE, "LIKE"},
    {detail::token::ORDER_BY, "ORDER BY"},
    {detail::token::GROUP_BY, "GROUP BY"},
    {detail::token::ASC, "ASC"},
    {detail::token::DESC, "DESC"},
    {detail::token::TOP, "LIMIT"},
    {detail::token::AS, "AS"},
    {detail::token::OFFSET, "OFFSET"},
    {detail::token::DISTINCT, "DISTINCT"},
    {detail::token::SET, "SET"},
    {detail::token::NOT_NULL, "NOT NULL"},
    {detail::token::PRIMARY_KEY, "PRIMARY KEY"},
    {detail::token::BEGIN, "BEGIN TRANSACTION"},
    {detail::token::COMMIT, "COMMIT TRANSACTION"},
    {detail::token::ROLLBACK, "ROLLBACK TRANSACTION"},
    {detail::token::START_QUOTE, "\""},
    {detail::token::END_QUOTE, "\""},
    {detail::token::STRING_QUOTE, "'"},
    {detail::token::NONE, ""}
  };
};

}
#endif //OOS_BASIC_DIALECT_HPP
