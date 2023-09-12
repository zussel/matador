#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

#include "matador/db/mysql/mysql_result.hpp"
#include "matador/db/mysql/mysql_exception.hpp"
#include "matador/db/mysql/mysql_constants.hpp"

#include <cstring>

namespace matador {

namespace mysql {

mysql_result::mysql_result(MYSQL *c)
  : affected_rows_((size_type)mysql_affected_rows(c))
{
  res_ = mysql_store_result(c);
  if (res_ == nullptr && mysql_errno(c) > 0) {
    throw_error(c, "mysql", "");
  } else if (res_) {
    rows_ = (size_type)mysql_num_rows(res_);
    fields_ = mysql_num_fields(res_);
  }
}

mysql_result::~mysql_result()
{
  if (res_) {
    mysql_free_result(res_);
  }
}

const char* mysql_result::column(size_type c) const
{
  if (row_) {
    return row_[c];
  } else {
    return nullptr;
  }
}

bool mysql_result::fetch()
{
  row_ = mysql_fetch_row(res_);
  if (!row_) {
    rows_ = 0;
    return false;
  }    
  return rows_-- > 0;
}

bool mysql_result::prepare_fetch()
{
  if (!fetch()) {
    return false;
  }

  return true;
}

bool mysql_result::finalize_fetch()
{
  return true;
}

mysql_result::size_type mysql_result::affected_rows() const
{
  return affected_rows_;
}

mysql_result::size_type mysql_result::result_rows() const
{
  return rows_;
}

mysql_result::size_type mysql_result::fields() const
{
  return fields_;
}

detail::result_impl::size_type mysql_result::reset_column_index() const
{
  return 0;
}

void mysql_result::read_value(const char */*id*/, size_type index, char &value)
{
  char *val = row_[index];
  if (strlen(val) > 1) {
    value = val[0];
  }
}

void mysql_result::read_value(const char */*id*/, size_type index, short &value)
{
  char *val = row_[index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  value = (short)strtol(val, &end, 10);
}

void mysql_result::read_value(const char */*id*/, size_type index, int &value)
{
  char *val = row_[index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  value = (int)strtol(val, &end, 10);
  // Todo: check error
}

void mysql_result::read_value(const char */*id*/, size_type index, long &value)
{
  char *val = row_[index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  value = strtol(val, &end, 10);
  // Todo: check error
}

void mysql_result::read_value(const char */*id*/, size_type index, long long &value)
{
  char *val = row_[index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  value = strtoll(val, &end, 10);
  // Todo: check error
}

void mysql_result::read_value(const char */*id*/, size_type index, unsigned char &value)
{
  char *val = row_[index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  value = (unsigned char)strtoul(val, &end, 10);
  // Todo: check error
}

void mysql_result::read_value(const char */*id*/, size_type index, unsigned short &value)
{
  char *val = row_[index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  value = (unsigned short)strtoul(val, &end, 10);
  // Todo: check error
}

void mysql_result::read_value(const char */*id*/, size_type index, unsigned int &value)
{
  char *val = row_[index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  value = (unsigned int)strtoul(val, &end, 10);
  // Todo: check error
}

void mysql_result::read_value(const char */*id*/, size_type index, unsigned long &value)
{
  char *val = row_[index];
  if (!val || strlen(val) == 0) {
    return;
  }
  char *end = nullptr;
  value = strtoul(val, &end, 10);
  // Todo: check error
}

void mysql_result::read_value(const char */*id*/, size_type index, unsigned long long &value)
{
  char *val = row_[index];
  if (!val || strlen(val) == 0) {
    return;
  }
  char *end = nullptr;
  value = strtoull(val, &end, 10);
  // Todo: check error
}

void mysql_result::read_value(const char */*id*/, size_type index, bool &value)
{
  char *val = row_[index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  value = strtoul(val, &end, 10) > 0;
  // Todo: check error
}

void mysql_result::read_value(const char */*id*/, size_type index, float &value)
{
  char *val = row_[index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  value = static_cast<float>(strtod(val, &end));
  // Todo: check error
}

void mysql_result::read_value(const char */*id*/, size_type index, double &value)
{
  char *val = row_[index];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  value = strtod(val, &end);
  // Todo: check error
}

void mysql_result::read_value(const char */*id*/, size_type index, char *value, size_t s)
{
  char *val = row_[index];
  size_t len = strlen(val);
  if (len > (size_t)s) {
    strncpy(value, val, (size_t)s);
    value[s-1] = '\n';
  } else {
    strcpy(value, val);
  }

}

void mysql_result::read_value(const char */*id*/, size_type index, std::string &value, size_t /*size*/)
{
  char *val = row_[index];
  value.assign(val);
}

void mysql_result::read_value(const char */*id*/, size_type index, std::string &value)
{
  char *val = row_[index];
  value.assign(val);
}

void mysql_result::read_value(const char */*id*/, size_type index, matador::date &value)
{
  char *val = row_[index];
  size_t len = strlen(val);
  if (val == nullptr || len == 0) {
    return;
  }
  value.set(val, matador::date_format::ISO8601);
}

void mysql_result::read_value(const char *id, size_type index, matador::time &value)
{
  std::string val;
  read_value(id, index, val);
  if (mysql::version < 50604) {
    // before mysql version 5.6.4 datetime
    // doesn't support fractional seconds,
    // so we use a datetime string here
    value = matador::time::parse(val, "%Y-%m-%dT%T");
  } else {
    value = matador::time::parse(val, "%Y-%m-%d %T.%f");
  }
}

void mysql_result::close() {}

}

}
