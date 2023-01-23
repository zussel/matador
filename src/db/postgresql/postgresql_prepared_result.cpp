#include "matador/db/postgresql/postgresql_prepared_result.hpp"
#include "matador/db/postgresql/postgresql_getvalue.hpp"

//#include "matador/utils/basic_identifier.hpp"

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

void postgresql_prepared_result::read_value(int index, int row, char &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, short &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, int &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, long &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, long long &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, unsigned char &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, unsigned short &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, unsigned int &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, unsigned long &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, unsigned long long &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, bool &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, float &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, double &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, matador::date &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, matador::time &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, char *x, size_t s)
{
  detail::get_value(res_, row, index, x, s);
}

void postgresql_prepared_result::read_value(int index, int row, std::string &x)
{
  detail::get_value(res_, row, index, x);
}

void postgresql_prepared_result::read_value(int index, int row, std::string &x, size_t s)
{
  detail::get_value(res_, row, index, x, s);
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