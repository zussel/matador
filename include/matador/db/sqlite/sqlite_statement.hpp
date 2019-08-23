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

#ifndef SQLITE_STATEMENT_HPP
#define SQLITE_STATEMENT_HPP

#include "matador/sql/statement_impl.hpp"

#include <string>
#include <vector>
#include <memory>

struct sqlite3_stmt;

namespace matador {

class varchar_base;
class basic_identifier;

namespace sqlite {

class sqlite_connection;

class sqlite_statement : public matador::detail::statement_impl
{
public:
  sqlite_statement(sqlite_connection &db, const std::string &stmt);
  ~sqlite_statement() override;

  void clear() override;
  detail::result_impl* execute() override;
  void reset() override;

protected:
  void serialize(const char *id, char &x) override;
  void serialize(const char *id, short &x) override;
  void serialize(const char *id, int &x) override;
  void serialize(const char *id, long &x) override;
  void serialize(const char *id, unsigned char &x) override;
  void serialize(const char *id, unsigned short &x) override;
  void serialize(const char *id, unsigned int &x) override;
  void serialize(const char *id, unsigned long &x) override;
  void serialize(const char *id, float &x) override;
  void serialize(const char *id, double &x) override;
  void serialize(const char *id, bool &x) override;
  void serialize(const char *id, char *x, size_t s) override;
  void serialize(const char *id, std::string &x, size_t s) override;
  void serialize(const char *id, std::string &x) override;
  void serialize(const char *id, matador::date &x) override;
  void serialize(const char *id, matador::time &x) override;
  void serialize(const char *id, basic_identifier &x) override;
  void serialize(const char *id, identifiable_holder&x, cascade_type) override;

private:
  sqlite_connection &db_;
  sqlite3_stmt *stmt_;

  std::vector<std::shared_ptr<std::string> > host_strings_;
};

}

}

#endif /* SQLITE_STATEMENT_HPP */
