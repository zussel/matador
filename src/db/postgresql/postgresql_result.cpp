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

void postgresql_result::read_value(const char *id, int index, int row, char &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, short &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, int &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, long &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, long long &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, unsigned char &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, unsigned short &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, unsigned int &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, unsigned long &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, unsigned long long &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, bool &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, float &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, double &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, char *value, long size)
{
  detail::get_value(res_, row, index, value, size);
}

void postgresql_result::read_value(const char *id, int index, int row, std::string &value, long size)
{
  detail::get_value(res_, row, index, value, size);
}

void postgresql_result::read_value(const char *id, int index, int row, std::string &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, matador::date &value)
{
  detail::get_value(res_, row, index, value);
}

void postgresql_result::read_value(const char *id, int index, int row, matador::time &value)
{
  detail::get_value(res_, row, index, value);
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