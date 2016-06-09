//
// Created by sascha on 2/25/16.
//

#ifndef OOS_BASIC_DIALECT_HPP
#define OOS_BASIC_DIALECT_HPP

#include "sql/types.hpp"
#include "sql/token.hpp"
#include "sql/token_list.hpp"
#include "sql/token_visitor.hpp"
#include "sql/basic_dialect_compiler.hpp"

#include <unordered_map>
#include <memory>
#include <string>
#include <list>

namespace oos {

class sql;

class basic_dialect : public token_visitor
{
public:
  enum t_compile_type {
    PREPARED,
    DIRECT
  };

public:
  explicit basic_dialect(detail::basic_dialect_compiler *compiler);
  virtual ~basic_dialect() {}

  std::string direct(const sql &s);
  std::string prepare(const sql &s);

  std::string build(sql &s, t_compile_type compile_type, bool reset);

  void reset();
  void compile(sql &s);
  void link(sql &s);

  std::string token_string(detail::token::t_token tok) const;

  virtual const char* type_string(data_type_t type) const = 0;

  t_compile_type compile_type() const;

  bool is_preparing() const;
  size_t bind_count() const;
  size_t column_count() const;

  size_t inc_bind_count();
  size_t dec_bind_count();

  size_t inc_column_count();
  size_t dec_column_count();

  void append_to_result(const std::string &part);
  std::string result() const;

  virtual void visit(const oos::detail::create &);
  virtual void visit(const oos::detail::drop &);
  virtual void visit(const oos::detail::select &);
  virtual void visit(const oos::detail::distinct &);
  virtual void visit(const oos::detail::update &);
  virtual void visit(const oos::detail::set &);
  virtual void visit(const oos::detail::as &);
  virtual void visit(const oos::detail::top &);
  virtual void visit(const oos::detail::remove &);
  virtual void visit(const oos::detail::values &values);
  virtual void visit(const oos::detail::basic_value &);
  virtual void visit(const oos::detail::order_by &);
  virtual void visit(const oos::detail::asc &);
  virtual void visit(const oos::detail::desc &);
  virtual void visit(const oos::detail::group_by &);
  virtual void visit(const oos::detail::insert &);
  virtual void visit(const oos::detail::from &);
  virtual void visit(const oos::detail::where &);
  virtual void visit(const oos::detail::basic_condition &);
  virtual void visit(const oos::detail::basic_column_condition &);
  virtual void visit(const oos::detail::basic_in_condition &);
  virtual void visit(const oos::columns &);
  virtual void visit(const oos::column &);
  virtual void visit(const oos::detail::typed_column &);
  virtual void visit(const oos::detail::identifier_column &);
  virtual void visit(const oos::detail::typed_varchar_column &);
  virtual void visit(const oos::detail::identifier_varchar_column &);
  virtual void visit(const oos::detail::basic_value_column &);
  virtual void visit(const oos::detail::begin &);
  virtual void visit(const oos::detail::commit &);
  virtual void visit(const oos::detail::rollback &);
  virtual void visit(const oos::detail::query &);

protected:
  void replace_token(detail::token::t_token tkn, const std::string &value);

private:
  std::string result_;
  size_t bind_count_ = 0;
  size_t column_count_ = 0;

  t_compile_type compile_type_;

  std::unique_ptr<detail::basic_dialect_compiler> compiler_;

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
