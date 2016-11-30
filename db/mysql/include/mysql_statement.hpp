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

#ifndef MYSQL_STATEMENT_HPP
#define MYSQL_STATEMENT_HPP

#include "sql/statement_impl.hpp"

#include "tools/identifier.hpp"

#ifdef WIN32
//#include <winsock2.h>
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

#include <string>
#include <vector>
#include <type_traits>
#include <tools/varchar.hpp>

namespace oos {

class varchar_base;
class time;
class date;

namespace mysql {

class mysql_connection;

class mysql_statement : public oos::detail::statement_impl
{
public:
  mysql_statement(mysql_connection &db, const oos::sql &stmt);
  virtual ~mysql_statement();

  virtual void clear();
  virtual detail::result_impl* execute();
  virtual void reset();
  
protected:
  virtual void serialize(const char *id, char &x);
  virtual void serialize(const char *id, short &x);
  virtual void serialize(const char *id, int &x);
  virtual void serialize(const char *id, long &x);
  virtual void serialize(const char *id, unsigned char &x);
  virtual void serialize(const char *id, unsigned short &x);
  virtual void serialize(const char *id, unsigned int &x);
  virtual void serialize(const char *id, unsigned long &x);
  virtual void serialize(const char *id, float &x);
  virtual void serialize(const char *id, double &x);
  virtual void serialize(const char *id, bool &x);
  virtual void serialize(const char *id, char *x, size_t s);
  virtual void serialize(const char *id, varchar_base &x);
  virtual void serialize(const char *id, std::string &x);
  virtual void serialize(const char *id, oos::date &x);
  virtual void serialize(const char *id, oos::time &x);
  virtual void serialize(const char *id, basic_identifier &x);
  virtual void serialize(const char *id, identifiable_holder&x, cascade_type);

private:
  template < class T >
  void bind_value(MYSQL_BIND &bind, enum_field_types type, T value, size_t)
  {
    if (bind.buffer == nullptr) {
      // allocating memory
      bind.buffer = new char[sizeof(T)];
    }
    *static_cast<T*>(bind.buffer) = value;
    bind.buffer_type = type;
	bind.buffer_length = sizeof(T);
    bind.is_null = 0;
	bind.is_unsigned = std::is_unsigned<T>::value;
	//std::cout << "\n bind value " << value << " (" << *static_cast<T*>(bind.buffer) << ")\n";
  }
  void bind_value(MYSQL_BIND &bind, enum_field_types type, const oos::date &x, size_t index);
  void bind_value(MYSQL_BIND &bind, enum_field_types type, const oos::time &x, size_t index);
  void bind_value(MYSQL_BIND &bind, enum_field_types type, size_t index);
  void bind_value(MYSQL_BIND &bind, enum_field_types type, const char *value, size_t size, size_t index);

private:
  size_t result_size;
  size_t host_size;
  std::vector<unsigned long> length_vector;
  MYSQL_STMT *stmt_ = nullptr;
  MYSQL_BIND *host_array = nullptr;
};

}

}

#endif /* MYSQL_STATEMENT_HPP */
