#ifndef OOS_DIALECT_TOKEN_HPP
#define OOS_DIALECT_TOKEN_HPP

//#include "matador/sql/export.hpp"

#include "matador/sql/token.hpp"
#include "matador/sql/condition.hpp"

namespace matador {

namespace detail {

/// @cond MATADOR_DEV

struct select : public token
{
  select() : token(SELECT) {}

  void accept(token_visitor &visitor) override;
};

struct begin : public token
{
  begin() : token(BEGIN) {}

  void accept(token_visitor &visitor) override;
};

struct commit : public token
{
  commit() : token(COMMIT) {}

  void accept(token_visitor &visitor) override;
};

struct rollback : public token
{
  rollback() : token(ROLLBACK) {}

  void accept(token_visitor &visitor) override;
};

struct drop : public table_name_token
{
  explicit drop(const std::string &t) : table_name_token(DROP, t) {}

  void accept(token_visitor &visitor) override;
};

struct create : public table_name_token
{
  explicit create(const std::string &t);

  void accept(token_visitor &visitor) override;
};

struct insert : public table_name_token
{
  explicit insert(std::string t);

  void accept(token_visitor &visitor) override;
};

struct update : public token
{
  update();

  void accept(token_visitor &visitor) override;
};

struct tablename : public table_name_token
{
  explicit tablename(std::string t);

  void accept(token_visitor &visitor) override;
};

struct remove : public token
{
  remove();

  void accept(token_visitor &visitor) override;
};

struct distinct : public token
{
  distinct() : token(DISTINCT) {}

  void accept(token_visitor &visitor) override;
};

struct set : public token
{
  set() : token(SET) {}

  void accept(token_visitor &visitor) override;
};

struct values : public token
{
  values() : token(VALUES) {}

  void push_back(const std::shared_ptr<value> &val) { values_.push_back(val); }

  void accept(token_visitor &visitor) override
  {
    return visitor.visit(*this);
  }

  std::vector<std::shared_ptr<value>> values_;
};

struct asc : public token
{
  asc() : token(ASC) {}

  void accept(token_visitor &visitor) override;
};

struct desc : public token
{
  desc() : token(DESC) {}

  void accept(token_visitor &visitor) override;
};

struct from : public table_name_token
{
  explicit from(const std::string &t);

  void accept(token_visitor &visitor) override;
};

struct top : public token
{
  explicit top(size_t lmt);

  void accept(token_visitor &visitor) override;

  size_t limit_;
};

struct as : public token
{
  explicit as(std::string a);

  void accept(token_visitor &visitor) override;

  std::string alias;
};

struct order_by : public token
{
  explicit order_by(std::string col);

  void accept(token_visitor &visitor) override;

  std::string column;
};

struct group_by : public token
{
  explicit group_by(std::string col);

  void accept(token_visitor &visitor) override;

  std::string column;
};

struct where : public token
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
