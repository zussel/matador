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
  : table_name_token(CREATE_TABLE, t)
{}

void create::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

insert::insert()
  : token(INSERT)
{}

void insert::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

into::into(std::string name)
: table_name_token(INTO, std::move(name))
{}

void into::accept(token_visitor &visitor)
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

tablename::tablename(std::string t)
  : table_name_token(TABLE, std::move(t))
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
  : table_name_token(FROM, t)
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

as::as(std::string a)
  : token(AS), alias(std::move(a))
{ }

void as::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

order_by::order_by(std::string col)
  : token(ORDER_BY), column(std::move(col))
{}

void order_by::accept(token_visitor &visitor)
{
  visitor.visit(*this);
}

group_by::group_by(std::string col)
  : token(GROUP_BY), column(std::move(col))
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