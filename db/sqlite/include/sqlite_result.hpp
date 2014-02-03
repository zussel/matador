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

#ifndef SQLITE_RESULT_HPP
#define SQLITE_RESULT_HPP

#include "database/result.hpp"
#include "database/row.hpp"

#include "tools/convert.hpp"

#include <vector>

namespace oos {

class row;
class object_atomizable;

namespace sqlite {

class sqlite_result : public result
{
private:
  sqlite_result(const sqlite_result&);
  sqlite_result& operator=(const sqlite_result&);

public:
  typedef result::size_type size_type;

public:
  sqlite_result();
  virtual ~sqlite_result();
  
  const char* column(size_type c) const;
  virtual bool fetch();
  virtual bool fetch(object *);
  size_type affected_rows() const;
  size_type result_rows() const;
  size_type fields() const;

  virtual int transform_index(int index) const;

  friend std::ostream& operator<<(std::ostream &out, const sqlite_result &res);

  void push_back(row *r);

protected:
  virtual void read(const char *id, char &x);
  virtual void read(const char *id, short &x);
  virtual void read(const char *id, int &x);
  virtual void read(const char *id, long &x);
  virtual void read(const char *id, unsigned char &x);
  virtual void read(const char *id, unsigned short &x);
  virtual void read(const char *id, unsigned int &x);
  virtual void read(const char *id, unsigned long &x);
  virtual void read(const char *id, bool &x);
  virtual void read(const char *id, float &x);
  virtual void read(const char *id, double &x);
  virtual void read(const char *id, char *x, int s);
  virtual void read(const char *id, varchar_base &x);
  virtual void read(const char *id, std::string &x);
  virtual void read(const char *id, object_base_ptr &x);
  virtual void read(const char *id, object_container &x);

  template < class T >
  void read_column(const char *, T &x)
  {
    std::string val = rows_.at(pos_)->str(result_index);
//    std::string val = rows_.at(pos_)->at<std::string>(result_index);
    convert(val, x);
  }

private:
  typedef std::vector<row*> row_vector_t;
  row_vector_t rows_;
  row_vector_t::size_type pos_;

  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  int result_size;
};

}

}

#endif /* SQLITE_RESULT_HPP */
