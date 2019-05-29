//
// Created by sascha on 29.05.19.
//

#include "matador/db/postgresql/postgresql_result.hpp"

namespace matador {
namespace postgresql {

postgresql_result::postgresql_result()
{

}

postgresql_result::~postgresql_result()
{

}

const char *postgresql_result::column(postgresql_result::size_type c) const
{
  return nullptr;
}

bool postgresql_result::fetch()
{
  return false;
}

postgresql_result::size_type postgresql_result::affected_rows() const
{
  return 0;
}

postgresql_result::size_type postgresql_result::result_rows() const
{
  return 0;
}

postgresql_result::size_type postgresql_result::fields() const
{
  return 0;
}

int postgresql_result::transform_index(int index) const
{
  return 0;
}

void postgresql_result::serialize(const char *id, char &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, short &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, int &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, long &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, unsigned char &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, unsigned short &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, unsigned int &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, unsigned long &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, bool &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, float &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, double &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, char *x, size_t s)
{
  result_impl::serialize(id, x, s);
}

void postgresql_result::serialize(const char *id, varchar_base &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, std::string &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, matador::date &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, matador::time &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, matador::basic_identifier &x)
{
  result_impl::serialize(id, x);
}

void postgresql_result::serialize(const char *id, matador::identifiable_holder &x, cascade_type)
{
  result_impl::serialize(id, x, < unnamed >);
}

bool postgresql_result::prepare_fetch()
{
  return false;
}

bool postgresql_result::finalize_fetch()
{
  return false;
}
}
}