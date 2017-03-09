//
// Created by sascha on 3/2/16.
//

#include "matador/sql/dialect_token.hpp"
#include "matador/sql/token_visitor.hpp"

namespace matador {

namespace detail {

void select::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

void rollback::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

void begin::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

void commit::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

void drop::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

create::create(const std::string &t)
  : token(CREATE_TABLE), table(t)
{}

void create::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

insert::insert(const std::string &t)
  : token(INSERT), table(t)
{}

void insert::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

update::update()
  : token(UPDATE)
{}

void update::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

tablename::tablename(const std::string &t)
  : token(TABLE)
  , tab(t)
{}

void tablename::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

remove::remove()
  : token(REMOVE)
{}

void remove::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

void distinct::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

void set::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

void asc::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

void desc::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

from::from(const std::string &t)
  : token(FROM), table(t)
{}

void from::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

top::top(size_t lmt)
  : token(TOP), limit_(lmt)
{}

void top::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

as::as(const std::string &a)
  : token(AS), alias(a)
{ }

void as::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

order_by::order_by(const std::string &col)
  : token(ORDER_BY), column(col)
{}

void order_by::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

group_by::group_by(const std::string &col)
  : token(GROUP_BY), column(col)
{}

void group_by::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

void where::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

}
}