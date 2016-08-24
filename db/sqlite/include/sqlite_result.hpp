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

#include "sql/result_impl.hpp"

#include <vector>

namespace oos {

class row;
class serializable;

namespace sqlite {

class sqlite_result : public detail::result_impl
{
private:
  sqlite_result(const sqlite_result&) = delete;
  sqlite_result& operator=(const sqlite_result&) = delete;

public:
  typedef detail::result_impl::size_type size_type;

public:
  sqlite_result();
  virtual ~sqlite_result();
  
  virtual const char* column(size_type c) const override;
  virtual bool fetch() override;
  virtual size_type affected_rows() const override;
  virtual size_type result_rows() const override;
  virtual size_type fields() const override;

  virtual int transform_index(int index) const override;

  void push_back(char **row_values, int column_count);

protected:
  virtual bool prepare_fetch() override;

  virtual bool finalize_fetch() override;

protected:
  virtual void serialize(const char*, char&) override;
  virtual void serialize(const char*, short&) override;
  virtual void serialize(const char*, int&) override;
  virtual void serialize(const char*, long&) override;
  virtual void serialize(const char*, unsigned char&) override;
  virtual void serialize(const char*, unsigned short&) override;
  virtual void serialize(const char*, unsigned int&) override;
  virtual void serialize(const char*, unsigned long&) override;
  virtual void serialize(const char*, bool&) override;
  virtual void serialize(const char*, float&) override;
  virtual void serialize(const char*, double&) override;
  virtual void serialize(const char*, char *, size_t) override;
  virtual void serialize(const char*, std::string&) override;
  virtual void serialize(const char*, oos::varchar_base&) override;
  virtual void serialize(const char*, oos::time&) override;
  virtual void serialize(const char*, oos::date&) override;
  virtual void serialize(const char*, oos::basic_identifier &x) override;
  virtual void serialize(const char*, oos::identifiable_holder &x, cascade_type) override;

private:
  friend class sqlite_connection;

private:

//  typedef std::vector<std::shared_ptr<char> > t_row;
  typedef std::vector<char*> t_row;
  typedef std::vector<t_row> t_result;

  t_result result_;
  t_result::size_type pos_ = 0;
  t_result::size_type column_ = 0;

  size_type fields_;
};

}

}

#endif /* SQLITE_RESULT_HPP */
