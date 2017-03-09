//
// Created by sascha on 3/2/16.
//

#ifndef OOS_DIALECT_TOKEN_HPP
#define OOS_DIALECT_TOKEN_HPP

#ifdef _MSC_VER
#ifdef oos_sql_EXPORTS
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

/// @cond OOS_DEV

struct OOS_SQL_API select : public token
{
  select() : token(SELECT) {}

  virtual void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API begin : public token
{
  begin() : token(BEGIN) {}

  virtual void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API commit : public token
{
  commit() : token(COMMIT) {}

  virtual void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API rollback : public token
{
  rollback() : token(ROLLBACK) {}

  virtual void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API drop : public token
{
  drop(const std::string &t) : token(DROP), table(t) {}

  virtual void accept(token_visitor &visitor) override;

  std::string table;
};

struct OOS_SQL_API create : public token
{
  create(const std::string &t);

  virtual void accept(token_visitor &visitor) override;

  std::string table;
};

struct OOS_SQL_API insert : public token
{
  insert(const std::string &t);

  virtual void accept(token_visitor &visitor) override;

  std::string table;
};

struct OOS_SQL_API update : public token
{
  update();

  virtual void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API tablename : public token
{
  tablename(const std::string &t);

  virtual void accept(token_visitor &visitor) override;

  std::string tab;
};

struct OOS_SQL_API remove : public token
{
  remove();

  virtual void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API distinct : public token
{
  distinct() : token(DISTINCT) {}

  virtual void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API set : public token
{
  set() : token(SET) {}

  virtual void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API values : public token
{
  values() : token(VALUES) {}

  void push_back(const std::shared_ptr<basic_value> &val) { values_.push_back(val); }

  virtual void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }

  std::vector<std::shared_ptr<basic_value>> values_;
};

struct OOS_SQL_API asc : public token
{
  asc() : token(ASC) {}

  virtual void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API desc : public token
{
  desc() : token(DESC) {}

  virtual void accept(token_visitor &visitor) override;
};

struct OOS_SQL_API from : public token
{
  from(const std::string &t);

  virtual void accept(token_visitor &visitor) override;

  std::string table;
};

struct OOS_SQL_API top : public token
{
  top(size_t lmt);

  virtual void accept(token_visitor &visitor) override;

  size_t limit_;
};

struct OOS_SQL_API as : public token
{
  as(const std::string &a);

  virtual void accept(token_visitor &visitor) override;

  std::string alias;
};

struct OOS_SQL_API order_by : public token
{
  order_by(const std::string &col);

  virtual void accept(token_visitor &visitor) override;

  std::string column;
};

struct OOS_SQL_API group_by : public token
{
  group_by(const std::string &col);

  virtual void accept(token_visitor &visitor) override;

  std::string column;
};

struct OOS_SQL_API where : public token
{
  template < class COND >
  explicit where(const COND &c)
    : token(token::WHERE)
    , cond(new COND(c))
  { }

  where(const std::shared_ptr<basic_condition> &c)
    : token(token::WHERE)
    , cond(c)
  { }

  virtual void accept(token_visitor &visitor) override;

  std::shared_ptr<basic_condition> cond;
};

/// @endcond

}

}
#endif //OOS_DIALECT_TOKEN_HPP
