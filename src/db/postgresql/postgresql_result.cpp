#include "matador/utils/string.hpp"
#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"

#include <cstring>

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
  return PQgetvalue(res_, row_index_, c);
}

bool postgresql_result::fetch()
{
  return ++row_index_ < rows_;
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

detail::result_impl::size_type postgresql_result::reset_column_index() const
{
  return 0;
}

void postgresql_result::read_value(const char */*id*/, size_type index, char &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, short &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, int &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, long &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, long long &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, unsigned char &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, unsigned short &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, unsigned int &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, unsigned long &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, unsigned long long &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, bool &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, float &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, double &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, char *value, size_t size)
{
  detail::get_value(res_, row_index_, index, value, size);
}

void postgresql_result::read_value(const char */*id*/, size_type index, std::string &value, size_t size)
{
  detail::get_value(res_, row_index_, index, value, size);
}

void postgresql_result::read_value(const char */*id*/, size_type index, std::string &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, matador::date &value)
{
  detail::get_value(res_, row_index_, index, value);
}

void postgresql_result::read_value(const char */*id*/, size_type index, matador::time &value)
{
  detail::get_value(res_, row_index_, index, value);
}

bool postgresql_result::prepare_fetch()
{
  return row_index_ + 1 <= rows_;
}

bool postgresql_result::finalize_fetch()
{
  ++row_index_;
  return true;
}

PGresult *postgresql_result::result_handle()
{
  return res_;
}

void postgresql_result::close() {}

}
}