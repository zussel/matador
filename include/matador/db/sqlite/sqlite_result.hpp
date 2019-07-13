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

#include "matador/sql/result_impl.hpp"

#include <vector>

namespace matador {

class row;
class serializable;

namespace sqlite {

class sqlite_result : public detail::result_impl
{
public:
  sqlite_result(const sqlite_result&) = delete;
  sqlite_result& operator=(const sqlite_result&) = delete;

public:
  typedef detail::result_impl::size_type size_type;

public:
  sqlite_result() = default;
  ~sqlite_result() override;
  
  const char* column(size_type c) const override;
  bool fetch() override;
  size_type affected_rows() const override;
  size_type result_rows() const override;
  size_type fields() const override;

  int transform_index(int index) const override;

  void push_back(char **row_values, int column_count);

protected:
  bool prepare_fetch() override;

  bool finalize_fetch() override;

protected:
  void serialize(const char*, char&) override;
  void serialize(const char*, short&) override;
  void serialize(const char*, int&) override;
  void serialize(const char*, long&) override;
  void serialize(const char*, unsigned char&) override;
  void serialize(const char*, unsigned short&) override;
  void serialize(const char*, unsigned int&) override;
  void serialize(const char*, unsigned long&) override;
  void serialize(const char*, bool&) override;
  void serialize(const char*, float&) override;
  void serialize(const char*, double&) override;
  void serialize(const char*, char *, size_t) override;
  void serialize(const char*, std::string&) override;
  void serialize(const char*, matador::varchar_base&) override;
  void serialize(const char*, matador::time&) override;
  void serialize(const char*, matador::date&) override;
  void serialize(const char*, matador::basic_identifier &x) override;
  void serialize(const char*, matador::identifiable_holder &x, cascade_type) override;

private:
  friend class sqlite_connection;

private:

//  typedef std::vector<std::shared_ptr<char> > t_row;
  typedef std::vector<char*> t_row;
  typedef std::vector<t_row> t_result;

  t_result result_;
  t_result::size_type pos_ = 0;
  t_result::size_type column_ = 0;

  size_type fields_ = 0;
};

}

}

#endif /* SQLITE_RESULT_HPP */
