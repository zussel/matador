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

class mysql_result : public result_impl
{
public:
  virtual ~mysql_result();
};

class mysql_static_result : public mysql_result
{
public:
  mysql_static_result();
  virtual ~mysql_static_result();

  virtual bool next();

  virtual row* current() const;

  void push_back(row *r);

private:
  typedef std::vector<row*> row_vector_t;
  row_vector_t rows_;
  row_vector_t::size_type pos_;
  
};

}

}

#endif /* MYSQL_RESULT_HPP */
