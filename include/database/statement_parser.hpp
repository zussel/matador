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

#ifndef STATEMENT_PARSER_HPP
#define STATEMENT_PARSER_HPP

#include <functional>
#include <string>
#include <stdexcept>

namespace oos {

class statement_parser
{
private:
  enum state_t {
    SQL_BEGIN,
    SQL_BEGIN_RESULT,
    SQL_RESULT,
    SQL_BEGIN_HOST,
    SQL_HOST,
    SQL_STRING
  };

public:
  typedef std::function<void (const std::string&, int)> param_func_t;

public:
  statement_parser(const param_func_t &result_func, const param_func_t &host_func,
             char result_indicator = '~', char host_indicator = ':');

  std::string parse(const std::string &sql);

private:
  state_t state_;

  char result_indicator_;
  char host_indicator_;
  
  param_func_t result_func_;
  param_func_t host_func_;
};

}

#endif /* STATEMENT_PARSER_HPP */
