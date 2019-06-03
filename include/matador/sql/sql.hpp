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

#ifndef SQL_HPP
#define SQL_HPP

#ifdef _MSC_VER
  #ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_SQL_API
#endif

#include "matador/sql/types.hpp"
#include "matador/sql/token.hpp"
#include "matador/sql/commands.hpp"
#include "matador/sql/token_list.hpp"

#include <string>
#include <map>
#include <list>
#include <vector>
#include <stdexcept>
#include <memory>

namespace matador {

/// @cond MATADOR_DEV

namespace detail {

class basic_dialect_compiler;
class basic_dialect_linker;
struct build_info;

}

class OOS_SQL_API sql
{
public:
  sql();
  ~sql();

  void append(const std::shared_ptr<detail::token> &tokptr);
  void append(detail::token *tok);
  void append(const sql &stmt);

  void reset(t_query_command command_type);

  static unsigned int type_size(data_type type);

  template < class T >
  static unsigned int type()
  {
    return data_type_traits<T>::type();
  }

private:
  friend class basic_dialect;
  friend struct detail::build_info;
  friend class detail::basic_dialect_compiler;
  friend class detail::basic_dialect_linker;
  template < class L, class R, class E >
  friend class condition;

  t_query_command command_type_;
  token_list_t token_list_;
};

namespace detail {

struct query : public token
{
  explicit query(const sql &s);

  void accept(token_visitor &visitor) override;

  sql sql_;
};

}
/// @endcond

}

#endif /* SQL_HPP */
