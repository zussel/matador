/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY {} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include "database/result.hpp"

#include <string>

namespace oos {

class session;
class result;
class row;
class database;

/// @cond OOS_DEV
class OOS_API statement_impl
{
public:
  // auto prepare
//  explicit statement_impl(const std::string &sql);
  virtual ~statement_impl();

  virtual void prepare(const std::string &sql);

  virtual bool step() = 0;

  virtual void reset(bool clear_bindings) = 0;

  virtual int column_count() const = 0;
  virtual const char* column_name(int i) const = 0;

  virtual const char* column_text(int i) const = 0;
  virtual int column_int(int i) const = 0;
  virtual double column_double(int i) const = 0;

  virtual int bind(int i, double value) = 0;
  virtual int bind(int i, int value) = 0;
  virtual int bind(int i, long value) = 0;
  virtual int bind(int i, unsigned int value) = 0;
  virtual int bind(int i, const char *value) = 0;
  virtual int bind_null(int i) = 0;
  
  std::string sql() const;

  virtual database& db() = 0;
  virtual const database& db() const = 0;

private:
  std::string sql_;
};

class OOS_API statement
{
public:
  explicit statement(session &db);
  statement(session &db, const std::string &sql);
  ~statement();

  result_ptr execute();
  result_ptr execute(const std::string &sql);

private:
  statement_impl *impl_;
  std::string sql_;
};
/// @endcond

}

#endif /* STATEMENT_HPP */
