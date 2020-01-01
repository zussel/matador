/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/string.hpp"

#include "matador/utils/identifier.hpp"

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

  result_index_ = 0;
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

int mysql_result::transform_index(int index) const
{
  return index;
}

void mysql_result::serialize(const char *, char &x)
{
  char *val = row_[result_index_++];
  if (strlen(val) > 1) {
    x = val[0];
  }
}

void mysql_result::serialize(const char *, short &x)
{
  char *val = row_[result_index_++];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (short)strtol(val, &end, 10);
}

void mysql_result::serialize(const char *, int &x)
{
  char *val = row_[result_index_++];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (int)strtol(val, &end, 10);
  // Todo: check error
}

void mysql_result::serialize(const char *, long &x)
{
  char *val = row_[result_index_++];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = strtol(val, &end, 10);
  // Todo: check error
}

void mysql_result::serialize(const char *, long long &x)
{
  char *val = row_[result_index_++];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = strtoll(val, &end, 10);
  // Todo: check error
}

void mysql_result::serialize(const char *, unsigned char &x)
{
  char *val = row_[result_index_++];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (unsigned char)strtoul(val, &end, 10);
  // Todo: check error
}

void mysql_result::serialize(const char *, unsigned short &x)
{
  char *val = row_[result_index_++];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (unsigned short)strtoul(val, &end, 10);
  // Todo: check error
}

void mysql_result::serialize(const char *, unsigned int &x)
{
  char *val = row_[result_index_++];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (unsigned int)strtoul(val, &end, 10);
  // Todo: check error
}

void mysql_result::serialize(const char *, unsigned long &x)
{
  char *val = row_[result_index_++];
  if (!val || strlen(val) == 0) {
    return;
  }
  char *end = nullptr;
  x = strtoul(val, &end, 10);
  // Todo: check error
}

void mysql_result::serialize(const char *, unsigned long long &x)
{
  char *val = row_[result_index_++];
  if (!val || strlen(val) == 0) {
    return;
  }
  char *end = nullptr;
  x = strtoull(val, &end, 10);
  // Todo: check error
}

void mysql_result::serialize(const char *, bool &x)
{
  char *val = row_[result_index_++];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = strtoul(val, &end, 10) > 0;
  // Todo: check error
}

void mysql_result::serialize(const char *, float &x)
{
  char *val = row_[result_index_++];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = (float)strtod(val, &end);
  // Todo: check error
}

void mysql_result::serialize(const char *, double &x)
{
  char *val = row_[result_index_++];
  if (strlen(val) == 0) {
    return;
  }
  char *end;
  x = strtod(val, &end);
  // Todo: check error
}

void mysql_result::serialize(const char *, char *x, size_t s)
{
  char *val = row_[result_index_++];
  size_t len = strlen(val);
  if (len > (size_t)s) {
    strncpy(x, val, (size_t)s);
    x[s-1] = '\n';
  } else {
    strcpy(x, val);
  }

}

void mysql_result::serialize(const char *, std::string &x, size_t)
{
  char *val = row_[result_index_++];
  x.assign(val);
}

void mysql_result::serialize(const char *, std::string &x)
{
  char *val = row_[result_index_++];
  x.assign(val);
}

void mysql_result::serialize(const char *, matador::date &x)
{
  char *val = row_[result_index_++];
  size_t len = strlen(val);
  if (val == nullptr || len == 0) {
    return;
  }
  x.set(val, matador::date_format::ISO8601);
}

void mysql_result::serialize(const char *id, matador::time &x)
{
  std::string val;
  serialize(id, val);
  if (mysql::version < 50604) {
    // before mysql version 5.6.4 datetime
    // doesn't support fractional seconds
    // so we use a datetime string here
    x = matador::time::parse(val, "%Y-%m-%dT%T");
  } else {
    x = matador::time::parse(val, "%Y-%m-%d %T.%f");
  }
}

void mysql_result::serialize(const char *id, matador::basic_identifier &x)
{
  x.serialize(id, *this);
}

void mysql_result::serialize(const char *id, matador::identifiable_holder &x, cascade_type)
{
  read_foreign_object(id, x);
}

}

}
