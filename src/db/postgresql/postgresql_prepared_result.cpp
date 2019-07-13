//
// Created by sascha on 11.06.19.
//
#include "matador/db/postgresql/postgresql_prepared_result.hpp"
#include "matador/db/postgresql/postgresql_getvalue.hpp"

#include "matador/utils/basic_identifier.hpp"

namespace matador {
namespace postgresql {

postgresql_prepared_result::postgresql_prepared_result(postgresql_statement *, PGresult *res)
  : res_(res)
  , affected_rows_(0)
  , rows_(PQntuples(res))
  , fields_(PQnfields(res))
{

}

postgresql_prepared_result::~postgresql_prepared_result()
{
  PQclear(res_);
}

const char *postgresql_prepared_result::column(postgresql_prepared_result::size_type c) const
{
  return PQgetvalue(res_,row_, c);
}

bool postgresql_prepared_result::fetch()
{
  return row_++ < rows_;
}

postgresql_prepared_result::size_type postgresql_prepared_result::affected_rows() const
{
  return affected_rows_;
}

postgresql_prepared_result::size_type postgresql_prepared_result::result_rows() const
{
  return rows_;
}

postgresql_prepared_result::size_type postgresql_prepared_result::fields() const
{
  return fields_;
}

int postgresql_prepared_result::transform_index(int index) const
{
  return index;
}

void postgresql_prepared_result::serialize(const char *, char &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, short &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, int &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, long &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, unsigned char &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, unsigned short &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, unsigned int &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, unsigned long &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, bool &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, float &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, double &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, char *x, size_t s)
{
  detail::get_value(res_, row_, column_++, x, s);
}

void postgresql_prepared_result::serialize(const char *, matador::date &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, matador::time &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, std::string &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *, varchar_base &x)
{
  detail::get_value(res_, row_, column_++, x);
}

void postgresql_prepared_result::serialize(const char *id, basic_identifier &x)
{
  x.serialize(id, *this);
}

void postgresql_prepared_result::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  read_foreign_object(id, x);
}

bool postgresql_prepared_result::needs_bind()
{
  return result_impl::needs_bind();
}

bool postgresql_prepared_result::finalize_bind()
{
  return result_impl::finalize_bind();
}

bool postgresql_prepared_result::prepare_fetch()
{
  column_ = 0;
  return row_ + 1 <= rows_;
}

bool postgresql_prepared_result::finalize_fetch()
{
  ++row_;
  return true;
}
}
}