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

#ifndef MYSQL_RESULT_HPP
#define MYSQL_RESULT_HPP

#include "database/result.hpp"

#include <vector>

namespace oos {

class row;

namespace mysql {

class mysql_result : public result
{
private:
  mysql_result(const mysql_result&);
  mysql_result& operator=(const mysql_result&);

public:
  typedef unsigned long size_type;

public:
  mysql_result(MYSQL *c);
  virtual ~mysql_result();
  
  void get(serializable *o);
  const char* column(size_type c) const;
  bool fetch();
  size_type affected_rows() const;
  size_type result_rows() const;
  size_type fields() const;

  friend std::ostream& operator<<(std::ostream &out, const mysql_result &res);

private:
  struct result_deleter
  {
    void operator()(MYSQL_RES *res) const {
      if (res) {
        mysql_free_result(res);
      }
    }
  };
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  MYSQL* conn;
  MYSQL_ROW row;
  MYSQL_RES *res;
  int result_index;
  int result_size;
};

}

}

#endif /* MYSQL_RESULT_HPP */
