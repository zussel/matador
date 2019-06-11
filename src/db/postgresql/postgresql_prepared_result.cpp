//
// Created by sascha on 11.06.19.
//
#include "matador/db/postgresql/postgresql_prepared_result.hpp"
#include "matador/db/postgresql/postgresql_getvalue.hpp"

#include "matador/utils/basic_identifier.hpp"

namespace matador {
namespace postgresql {

postgresql_prepared_result::postgresql_prepared_result(postgresql_statement *owner, PGresult *s)
  : res_(s)
{

}

postgresql_prepared_result::~postgresql_prepared_result()
{

}

const char *postgresql_prepared_result::column(postgresql_prepared_result::size_type c) const
{
  return nullptr;
}

bool postgresql_prepared_result::fetch()
{
  return false;
}

postgresql_prepared_result::size_type postgresql_prepared_result::affected_rows() const
{
  return 0;
}

postgresql_prepared_result::size_type postgresql_prepared_result::result_rows() const
{
  return 0;
}

postgresql_prepared_result::size_type postgresql_prepared_result::fields() const
{
  return 0;
}

int postgresql_prepared_result::transform_index(int index) const
{
  return 0;
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
}
}