//
// Created by sascha on 3/2/16.
//

#ifndef OOS_DIALECT_TOKEN_HPP
#define OOS_DIALECT_TOKEN_HPP

#include "sql/token.hpp"
#include "sql/condition.hpp"

namespace oos {

namespace detail {

struct select : public token
{
  select() : token(SELECT) {}

  virtual std::string compile(basic_dialect &d) const override;
};

struct begin : public token
{
  begin() : token(BEGIN) {}

  virtual std::string compile(basic_dialect &d) const override;
};

struct commit : public token
{
  commit() : token(COMMIT) {}

  virtual std::string compile(basic_dialect &d) const override;
};

struct rollback : public token
{
  rollback() : token(ROLLBACK) {}

  virtual std::string compile(basic_dialect &d) const override;
};

struct drop : public token
{
  drop(const std::string &t) : token(DROP), table(t) {}

  virtual std::string compile(basic_dialect &d) const override;

  std::string table;
};

struct create : public token
{
  create(const std::string &t);

  virtual std::string compile(basic_dialect &d) const override;

  std::string table;
};

struct insert : public token
{
  insert(const std::string &t);

  virtual std::string compile(basic_dialect &d) const override;

  std::string table;
};

struct update : public token
{
  update(const std::string &t);

  virtual std::string compile(basic_dialect &d) const override;

  std::string table;
};

struct remove : public token
{
  remove(const std::string &t);

  virtual std::string compile(basic_dialect &d) const override;

  std::string table;
};

struct distinct : public token
{
  distinct() : token(DISTINCT) {}

  virtual std::string compile(basic_dialect &d) const override;
};

struct set : public token
{
  set() : token(SET) {}

  virtual std::string compile(basic_dialect &d) const override;
};

struct values : public token
{
  values() : token(VALUES) {}

  void push_back(const std::shared_ptr<basic_value> &val) { values_.push_back(val); }

  virtual std::string compile(basic_dialect &d) const override
  {
    return d.compile(*this);
  }

  std::vector<std::shared_ptr<basic_value>> values_;
};

struct asc : public token
{
  asc() : token(ASC) {}

  virtual std::string compile(basic_dialect &d) const override;
};

struct desc : public token
{
  desc() : token(DESC) {}

  virtual std::string compile(basic_dialect &d) const override;
};

struct from : public token
{
  from(const std::string &t);

  virtual std::string compile(basic_dialect &d) const override;

  std::string table;
};

struct top : public token
{
  top(size_t lmt);

  virtual std::string compile(basic_dialect &d) const override;

  size_t limit_;
};

struct as : public token
{
  as(const std::string &a);

  virtual std::string compile(basic_dialect &d) const override;

  std::string alias;
};

struct order_by : public token
{
  order_by(const std::string &col);

  virtual std::string compile(basic_dialect &d) const override;

  std::string column;
};

struct group_by : public token
{
  group_by(const std::string &col);

  virtual std::string compile(basic_dialect &d) const override;

  std::string column;
};

struct where : public token
{
  template < class COND >
  where(const COND &c)
    : token(WHERE)
    , cond(new COND(c))
  {}

  virtual std::string compile(basic_dialect &d) const override;

  std::shared_ptr<basic_condition> cond;
};

}

}
#endif //OOS_DIALECT_TOKEN_HPP
