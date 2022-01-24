#ifndef OOS_DIALECT_TOKEN_HPP
#define OOS_DIALECT_TOKEN_HPP

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

#include "matador/sql/token.hpp"
#include "matador/sql/condition.hpp"

namespace matador {

namespace detail {

/// @cond MATADOR_DEV

struct OOS_SQL_API select : public token
{
  select() : token(SELECT) {}

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API begin : public token
{
  begin() : token(BEGIN) {}

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API commit : public token
{
  commit() : token(COMMIT) {}

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API rollback : public token
{
  rollback() : token(ROLLBACK) {}

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API drop : public table_name_token
{
  explicit drop(const std::string &t) : table_name_token(DROP, t) {}

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API create : public table_name_token
{
  explicit create(const std::string &t);

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API insert : public table_name_token
{
  explicit insert(std::string t);

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API update : public token
{
  update();

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API tablename : public table_name_token
{
  explicit tablename(std::string t);

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API remove : public token
{
  remove();

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API distinct : public token
{
  distinct() : token(DISTINCT) {}

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API set : public token
{
  set() : token(SET) {}

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API values : public token
{
  values() : token(VALUES) {}

  void push_back(const std::shared_ptr<value> &val) { values_.push_back(val); }

  void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }

  std::vector<std::shared_ptr<value>> values_;
};

struct OOS_SQL_API asc : public token
{
  asc() : token(ASC) {}

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API desc : public token
{
  desc() : token(DESC) {}

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API from : public table_name_token
{
  explicit from(const std::string &t);

  void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API top : public token
{
  explicit top(size_t lmt);

  void accept(token_visitor &visitor) override;

  size_t limit_;
};

struct OOS_SQL_API as : public token
{
  explicit as(std::string a);

  void accept(token_visitor &visitor) override;

  std::string alias;
};

struct OOS_SQL_API order_by : public token
{
  explicit order_by(std::string col);

  void accept(token_visitor &visitor) override;

  std::string column;
};

struct OOS_SQL_API group_by : public token
{
  explicit group_by(std::string col);

  void accept(token_visitor &visitor) override;

  std::string column;
};

struct OOS_SQL_API where : public token
{
  template < class COND >
  explicit where(const COND &c)
    : token(token::WHERE)
    , cond(new COND(c))
  { }

  explicit where(std::shared_ptr<basic_condition> c)
    : token(token::WHERE)
    , cond(std::move(c))
  { }

  void accept(token_visitor &visitor) override;

  std::shared_ptr<basic_condition> cond;
};

/// @endcond

}

}
#endif //OOS_DIALECT_TOKEN_HPP
