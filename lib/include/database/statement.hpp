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

#ifdef _MSC_VER
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

#include "object/object_atomizer.hpp"

#include <string>
#include <functional>

namespace oos {

class result;
class serializable;
class sql;
class object_base_producer;

namespace detail {
  class statement_impl;
}

    /// @cond OOS_DEV
class OOS_API statement
{
public:
  statement
  virtual ~statement();

  virtual void clear() = 0;

  virtual void prepare(const sql &s) = 0;

  virtual result execute() = 0;

  virtual void reset() = 0;
  
  int bind(serializable *o);

  template < class T >
  int bind(unsigned long i, const T &val)
  {
    host_index = i;
    write("", val);
    return host_index;
  }

  std::string str() const;

protected:
  void str(const std::string &s);

protected:
  int host_index;

private:
  std::string sql_;

  oos::detail::statement_impl *p = nullptr;

};

/// @endcond

}

#endif /* STATEMENT_HPP */
