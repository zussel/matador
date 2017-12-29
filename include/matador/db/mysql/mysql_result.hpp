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

#include "matador/sql/result_impl.hpp"

#ifdef _MSC_VER
//#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <vector>

namespace matador {

class row;
class serializer;

namespace mysql {

class mysql_result : public detail::result_impl
{
private:
  mysql_result(const mysql_result&) = delete;
  mysql_result& operator=(const mysql_result&) = delete;

public:
  typedef detail::result_impl::size_type size_type;

public:
  mysql_result(MYSQL *c);
  virtual ~mysql_result();

  virtual const char* column(size_type c) const override;
  virtual bool fetch() override;
  
  virtual size_type affected_rows() const override;
  virtual size_type result_rows() const override;
  virtual size_type fields() const override;

  virtual int transform_index(int index) const override;

protected:
  virtual void serialize(const char *id, char &x) override;
  virtual void serialize(const char *id, short &x) override;
  virtual void serialize(const char *id, int &x) override;
  virtual void serialize(const char *id, long &x) override;
  virtual void serialize(const char *id, unsigned char &x) override;
  virtual void serialize(const char *id, unsigned short &x) override;
  virtual void serialize(const char *id, unsigned int &x) override;
  virtual void serialize(const char *id, unsigned long &x) override;
  virtual void serialize(const char *id, bool &x) override;
  virtual void serialize(const char *id, float &x) override;
  virtual void serialize(const char *id, double &x) override;
  virtual void serialize(const char *id, char *x, size_t s) override;
  virtual void serialize(const char *id, varchar_base &x) override;
  virtual void serialize(const char *id, std::string &x) override;
  virtual void serialize(const char *id, matador::date &x) override;
  virtual void serialize(const char *id, matador::time &x) override;
  virtual void serialize(const char *id, matador::basic_identifier &x) override;
  virtual void serialize(const char *id, matador::identifiable_holder &x, cascade_type) override;

protected:
  virtual bool prepare_fetch() override;
  virtual bool finalize_fetch() override;

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
  size_type rows_;
  size_type fields_;
  MYSQL_ROW row_;
  MYSQL_RES *res_;
};

}

}

#endif /* MYSQL_RESULT_HPP */
