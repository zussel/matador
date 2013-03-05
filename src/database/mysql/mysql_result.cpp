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

#include "database/mysql/mysql_result.hpp"
#include "database/row.hpp"

namespace oos {

namespace mysql {

mysql_result::mysql_result(MYSQL *c)
  : affected_rows_(mysql_affected_rows(c))
  , rows(0)
  , fields_(0)
  , conn(c)
  , res(0)
  , result_index(0)
  , result_size(0)
{
  res = mysql_store_result(c);
  if (res == 0 && mysql_errno(c) > 0) {
    printf("Error %u: %s\n", mysql_errno(c), mysql_error(c));
    exit(1);
  } else if (res) {
    rows = mysql_num_rows(res);
    fields_ = mysql_num_fields(res);
  }
}
mysql_result::~mysql_result()
{
  if (res) {
    mysql_free_result(res);
  }
}

void mysql_result::get(object_atomizable *)
{
  result_index = 0;
//  o->deserialize(*this);
}

const char* mysql_result::column(size_type c) const
{
  if (row) {
    return row[c];
  } else {
    return 0;
  }
}

bool mysql_result::fetch()
{
  row = mysql_fetch_row(res);
  if (!row) {
    rows = 0;
    return false;
  }    
  return rows-- > 0;
}

mysql_result::size_type mysql_result::affected_rows() const
{
  return affected_rows_;
}

mysql_result::size_type mysql_result::result_rows() const
{
  return rows;
}

mysql_result::size_type mysql_result::fields() const
{
  return fields_;
}

}

}
