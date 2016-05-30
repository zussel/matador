//
// Created by sascha on 2/25/16.
//

#ifndef OOS_BASIC_DIALECT_HPP
#define OOS_BASIC_DIALECT_HPP

#include "sql/types.hpp"
#include "sql/token.hpp"

#include <unordered_map>
#include <memory>
#include <string>
#include <list>

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
struct as;
struct order_by;
struct group_by;
struct begin;
struct commit;
struct rollback;
struct sql_token;

}

class basic_dialect
{
public:
  enum t_compile_type {
    PREPARED,
    DIRECT
  };

public:
  virtual ~basic_dialect() {}

  std::string direct(const sql &s);
  std::string prepare(const sql &s);

  virtual void parse() const = 0;

  std::string token(detail::token::t_token tok) const { return tokens.at(tok); }

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
  virtual std::string compile(const oos::detail::as &) = 0;
  virtual std::string compile(const oos::detail::begin &) = 0;
  virtual std::string compile(const oos::detail::commit &) = 0;
  virtual std::string compile(const oos::detail::rollback &) = 0;
  virtual std::string compile(const oos::detail::sql_token &) = 0;

  t_compile_type compile_type() const { return compile_type_; }

  bool is_preparing() const;
  virtual int bind_count() const { return 0; }
  virtual int column_count() const { return 0; }


protected:
  void replace_token(detail::token::t_token tkn, const std::string &value);

private:
  t_compile_type compile_type_;
  typedef std::unordered_map<detail::token::t_token, std::string, std::hash<int>> t_token_map;
  t_token_map tokens {
    {detail::token::CREATE_TABLE, "CREATE TABLE"},
    {detail::token::DROP, "DROP TABLE"},
    {detail::token::DELETE, "DELETE FROM"},
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
    {detail::token::NONE, ""}
  };
};

}
#endif //OOS_BASIC_DIALECT_HPP
