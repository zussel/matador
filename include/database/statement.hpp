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

#include <string>

namespace oos {

class database;
class result;
class row;

class OOS_API statement_impl
{
public:
  virtual ~statement_impl();

  virtual result* execute(const std::string &sql) = 0;
  virtual row* step() = 0;
/*
  virtual void create() = 0;
  virtual void destroy() = 0;
*/


};

class OOS_API statement
{
public:
  explicit statement(database &db);
  statement(database &db, const std::string &sql);
  ~statement();

  result* execute(const std::string &sql);

  row* step();

private:
  statement_impl *impl_;
};

}

#endif /* STATEMENT_HPP */
