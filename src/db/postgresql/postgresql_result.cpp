//
// Created by sascha on 29.05.19.
//

#include <cstring>

#include "matador/utils/string.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/basic_identifier.hpp"

#include "matador/db/postgresql/postgresql_result.hpp"
#include "matador/db/postgresql/postgresql_getvalue.hpp"

namespace matador {
namespace postgresql {

postgresql_result::postgresql_result(PGresult *res)
  : affected_rows_(0)
  , rows_(PQntuples(res))
  , fields_(PQnfields(res))
  , res_(res)
{
}

postgresql_result::~postgresql_result()
{
  if (res_) {
    PQclear(res_);
  }
}

const char *postgresql_result::column(postgresql_result::size_type c) const
{
  return PQgetvalue(res_, pos_, c);
}

bool postgresql_result::fetch()
{
  return ++pos_ < rows_;
}

postgresql_result::size_type postgresql_result::affected_rows() const
{
  return affected_rows_;
}

postgresql_result::size_type postgresql_result::result_rows() const
{
  return rows_;
}

postgresql_result::size_type postgresql_result::fields() const
{
  return fields_;
}

int postgresql_result::transform_index(int index) const
{
  return index;
}

void postgresql_result::serialize(const char *, char &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, short &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, int &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, long &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, unsigned char &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, unsigned short &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, unsigned int &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, unsigned long &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, bool &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, float &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, double &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, char *x, size_t s)
{
  detail::get_value(res_, pos_, column_++, x, s);
}

void postgresql_result::serialize(const char *, std::string &x, size_t s)
{
  detail::get_value(res_, pos_, column_++, x, s);
}

void postgresql_result::serialize(const char *, std::string &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, matador::date &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *, matador::time &x)
{
  detail::get_value(res_, pos_, column_++, x);
}

void postgresql_result::serialize(const char *id, matador::basic_identifier &x)
{
  x.serialize(id, *this);
}

void postgresql_result::serialize(const char *id, matador::identifiable_holder &x, cascade_type)
{
  read_foreign_object(id, x);
}

bool postgresql_result::prepare_fetch()
{
  column_ = 0;
  return pos_ + 1 <= rows_;
}

bool postgresql_result::finalize_fetch()
{
  ++pos_;
  return true;
}

PGresult *postgresql_result::result_handle()
{
  return res_;
}

}
}