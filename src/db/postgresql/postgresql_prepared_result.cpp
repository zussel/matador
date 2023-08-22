#include "matador/db/postgresql/postgresql_prepared_result.hpp"
#include "matador/db/postgresql/postgresql_getvalue.hpp"

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
  return PQgetvalue(res_,row_index_, c);
}

bool postgresql_prepared_result::fetch()
{
  return row_index_++ < rows_;;
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

detail::result_impl::size_type postgresql_prepared_result::reset_column_index() const
{
  return 0;
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, char &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, short &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, int &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, long &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, long long &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, unsigned char &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, unsigned short &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, unsigned int &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, unsigned long &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, unsigned long long &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, bool &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, float &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, double &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, matador::date &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, matador::time &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, char *x, size_t size)
{
  detail::get_value(res_, row_index_, index, x, size);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, std::string &x)
{
  detail::get_value(res_, row_index_, index, x);
}

void postgresql_prepared_result::read_value(const char */*id*/, size_type index, std::string &x, size_t size)
{
  detail::get_value(res_, row_index_, index, x, size);
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
  return row_index_ + 1 <= rows_;
}

bool postgresql_prepared_result::finalize_fetch()
{
  ++row_index_;
  return true;
}
}
}