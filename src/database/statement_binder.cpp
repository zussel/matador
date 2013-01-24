#include "database/statement_binder.hpp"
#include "database/statement.hpp"

#include "object/object.hpp"

namespace oos {

statement_binder::statement_binder()
{}

statement_binder::~statement_binder()
{}

void statement_binder::bind(const statement_ptr &stmt, object *o, bool bind_id)
{
  stmt_ = stmt;

  column_ = 0;
  // bind parameter
  o->serialize(*this);

  if (bind_id) {
    stmt_->bind(++column_, (int)o->id());
  }
}

void statement_binder::write(const char*, char x)
{
  stmt_->bind(++column_, x);
}

void statement_binder::write(const char*, float x)
{
  stmt_->bind(++column_, x);
}

void statement_binder::write(const char*, double x)
{
  stmt_->bind(++column_, x);
}

void statement_binder::write(const char*, short x)
{
  stmt_->bind(++column_, x);
}

void statement_binder::write(const char*, int x)
{
  stmt_->bind(++column_, x);
}

void statement_binder::write(const char*, long x)
{
  stmt_->bind(++column_, (int)x);
}

void statement_binder::write(const char*, unsigned char x)
{
  stmt_->bind(++column_, x);
}

void statement_binder::write(const char*, unsigned short x)
{
  stmt_->bind(++column_, x);
}

void statement_binder::write(const char*, unsigned int x)
{
  stmt_->bind(++column_, x);
}

void statement_binder::write(const char*, unsigned long x)
{
  stmt_->bind(++column_, (unsigned int)x);
}

void statement_binder::write(const char*, bool x)
{
  stmt_->bind(++column_, x);
}

void statement_binder::write(const char*, const char *x, int)
{
  stmt_->bind(++column_, x, strlen(x));
}

void statement_binder::write(const char*, const std::string &x)
{
  stmt_->bind(++column_, x.c_str(), x.size());
}

void statement_binder::write(const char*, const varchar_base &x)
{
  stmt_->bind(++column_, x.c_str(), x.size());
}

void statement_binder::write(const char*, const object_base_ptr &x)
{
  if (x.ptr()) {
    stmt_->bind(++column_, (int)x.id());
  } else {
    stmt_->bind_null(++column_);
  }
}

void statement_binder::write(const char*, const object_container &)
{
}

}
