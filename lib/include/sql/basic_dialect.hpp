//
// Created by sascha on 2/25/16.
//

#ifndef OOS_BASIC_DIALECT_HPP
#define OOS_BASIC_DIALECT_HPP

#include "sql/types.hpp"
#include "sql/token.hpp"
#include "sql/token_list.hpp"
#include "sql/token_visitor.hpp"

#include <unordered_map>
#include <memory>
#include <string>
#include <list>
#include <stack>

namespace oos {

class sql;
class basic_dialect;

namespace detail {

class basic_dialect_compiler;
class basic_dialect_linker;

struct build_info {
  build_info(const sql &s, basic_dialect *d);

  const sql &stmt;
  basic_dialect *dialect;
  token_list_t tokens_;
  token_list_t::iterator current;
  std::string result;
};

}

class basic_dialect
{
public:
  enum t_compile_type {
    PREPARED,
    DIRECT
  };

public:
  explicit basic_dialect(detail::basic_dialect_compiler *compiler, detail::basic_dialect_linker *linker);
  virtual ~basic_dialect();

  std::string direct(const sql &s);
  std::string prepare(const sql &s);

  std::string build(const sql &s, t_compile_type compile_type);

  virtual const char* type_string(data_type_t type) const = 0;

  t_compile_type compile_type() const;

  bool is_preparing() const;
  size_t bind_count() const;
  size_t column_count() const;

  size_t inc_bind_count();
  size_t inc_bind_count(size_t val);
  size_t dec_bind_count();

  size_t inc_column_count();
  size_t dec_column_count();

  std::string token_at(detail::token::t_token tok) const;

  void append_to_result(const std::string &part);

protected:
  friend class detail::basic_dialect_compiler;
  friend class detail::basic_dialect_linker;

  void replace_token(detail::token::t_token tkn, const std::string &value);

  void push(const sql &s);
  void pop();
  detail::build_info& top();

private:
  void compile();
  void link();

  size_t bind_count_ = 0;
  size_t column_count_ = 0;

  t_compile_type compile_type_;

  detail::basic_dialect_compiler* compiler_;
  detail::basic_dialect_linker* linker_;

  std::stack<detail::build_info> build_info_stack_;

  typedef std::unordered_map<detail::token::t_token, std::string, std::hash<int>> t_token_map;
  t_token_map tokens {
    {detail::token::CREATE_TABLE, "CREATE TABLE"},
    {detail::token::DROP, "DROP TABLE"},
    {detail::token::DELETE, "DELETE"},
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
