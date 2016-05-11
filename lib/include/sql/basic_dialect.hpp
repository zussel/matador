//
// Created by sascha on 2/25/16.
//

#ifndef OOS_BASIC_DIALECT_HPP
#define OOS_BASIC_DIALECT_HPP

#include "sql/types.hpp"

#include <unordered_map>
#include <string>

namespace oos {

class sql;
struct column;
struct columns;

namespace detail {

struct select;
struct insert;
struct update;
struct remove;
struct typed_column;
struct identifier_column;
struct typed_varchar_column;
struct values;
struct basic_value;
struct distinct;
struct set;
struct asc;
struct desc;
struct from;
struct where;
struct basic_condition;
struct create;
struct drop;
struct top;
struct order_by;
struct group_by;
struct begin;
struct commit;
struct rollback;

}

class basic_dialect
{
public:
  enum t_compile_type {
    PREPARED,
    DIRECT
  };

  enum t_token
  {
    CREATE_TABLE = 0,
    DROP,
    DELETE,
    INSERT,
    VALUES,
    VALUE,
    UPDATE,
    SELECT,
    COLUMNS,
    COLUMN,
    FROM,
    WHERE,
    AND,
    OR,
    ORDER_BY,
    GROUP_BY,
    ASC,
    DESC,
    TOP,
    OFFSET,
    DISTINCT,
    CONDITION,
    SET,
    NOT_NULL,
    PRIMARY_KEY,
    BEGIN,
    COMMIT,
    ROLLBACK
  };

public:
  virtual ~basic_dialect() {}

  std::string direct(const sql &s);
  std::string prepare(const sql &s);

  std::string token(t_token tok) const { return tokens.at(tok); }

  virtual const char* type_string(data_type_t type) const = 0;

  virtual std::string compile(const oos::detail::create &) = 0;
  virtual std::string compile(const oos::detail::drop &) = 0;
  virtual std::string compile(const oos::detail::select &) = 0;
  virtual std::string compile(const oos::detail::distinct &) = 0;
  virtual std::string compile(const oos::detail::update &) = 0;
  virtual std::string compile(const oos::detail::set &) = 0;
  virtual std::string compile(const oos::columns &) = 0;
  virtual std::string compile(const oos::column &) = 0;
  virtual std::string compile(const oos::detail::typed_column &) = 0;
  virtual std::string compile(const oos::detail::identifier_column &) = 0;
  virtual std::string compile(const oos::detail::typed_varchar_column &) = 0;
  virtual std::string compile(const oos::detail::from &) = 0;
  virtual std::string compile(const oos::detail::where &) = 0;
  virtual std::string compile(const oos::detail::basic_condition &) = 0;
  virtual std::string compile(const oos::detail::order_by &) = 0;
  virtual std::string compile(const oos::detail::asc &) = 0;
  virtual std::string compile(const oos::detail::desc &) = 0;
  virtual std::string compile(const oos::detail::group_by &) = 0;
  virtual std::string compile(const oos::detail::insert &) = 0;
  virtual std::string compile(const oos::detail::values &) = 0;
  virtual std::string compile(const oos::detail::basic_value &) = 0;
  virtual std::string compile(const oos::detail::remove &) = 0;
  virtual std::string compile(const oos::detail::top &) = 0;
  virtual std::string compile(const oos::detail::begin &) = 0;
  virtual std::string compile(const oos::detail::commit &) = 0;
  virtual std::string compile(const oos::detail::rollback &) = 0;

  t_compile_type compile_type() const { return compile_type_; }

  bool is_preparing() const;
  virtual int bind_count() const { return 0; }
  virtual int column_count() const { return 0; }


protected:
  void replace_token(t_token tkn, const std::string &value);

private:
  t_compile_type compile_type_;
  typedef std::unordered_map<t_token, std::string, std::hash<int>> t_token_map;
  t_token_map tokens {
    {basic_dialect::CREATE_TABLE, "CREATE TABLE"},
    {basic_dialect::DROP, "DROP TABLE"},
    {basic_dialect::DELETE, "DELETE FROM"},
    {basic_dialect::INSERT, "INSERT INTO"},
    {basic_dialect::VALUES, "VALUES"},
    {basic_dialect::UPDATE, "UPDATE"},
    {basic_dialect::SELECT, "SELECT"},
    {basic_dialect::COLUMNS, "COLUMNS"},
    {basic_dialect::COLUMN, "COLUMN"},
    {basic_dialect::FROM, "FROM"},
    {basic_dialect::WHERE, "WHERE"},
    {basic_dialect::AND, "AND"},
    {basic_dialect::OR, "OR"},
    {basic_dialect::ORDER_BY, "ORDER BY"},
    {basic_dialect::GROUP_BY, "GROUP BY"},
    {basic_dialect::ASC, "ASC"},
    {basic_dialect::DESC, "DESC"},
    {basic_dialect::TOP, "LIMIT"},
    {basic_dialect::OFFSET, "OFFSET"},
    {basic_dialect::DISTINCT, "DISTINCT"},
    {basic_dialect::SET, "SET"},
    {basic_dialect::NOT_NULL, "NOT NULL"},
    {basic_dialect::PRIMARY_KEY, "PRIMARY KEY"},
    {basic_dialect::BEGIN, "BEGIN TRANSACTION"},
    {basic_dialect::COMMIT, "COMMIT TRANSACTION"},
    {basic_dialect::ROLLBACK, "ROLLBACK TRANSACTION"}
  };
};

}
#endif //OOS_BASIC_DIALECT_HPP
