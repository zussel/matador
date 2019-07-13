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

#ifndef TOKEN_HPP
#define TOKEN_HPP

#ifdef _MSC_VER
#ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
  #pragma warning(disable: 4355)
#else
#define OOS_SQL_API
#endif

#include "matador/sql/types.hpp"

#include <memory>
#include <sstream>
#include <vector>
#include <iterator>

namespace matador {

class basic_dialect;
class token_visitor;

namespace detail {

/// @cond MATADOR_DEV
struct OOS_SQL_API token
{
  enum t_token
  {
    CREATE_TABLE = 0,
    DROP,
    REMOVE,
    INSERT,
    VALUES,
    VALUE,
    UPDATE,
    SELECT,
    COLUMNS,
    COLUMN,
    FROM,
    WHERE,
    AND,
    OR,
    ORDER_BY,
    GROUP_BY,
    ASC,
    DESC,
    TOP,
    AS,
    OFFSET,
    DISTINCT,
    CONDITION,
    SET,
    NOT_NULL,
    PRIMARY_KEY,
    BEGIN,
    COMMIT,
    TABLE,
    ROLLBACK,
    START_QUOTE,
    END_QUOTE,
    STRING_QUOTE,
    NONE
  };

  explicit token(t_token tok);
  virtual ~token() = default;

  virtual void accept(token_visitor &visitor) = 0;

  t_token type;
};

struct OOS_SQL_API table_name_token : public token
{
  table_name_token(t_token tok, std::string tablename);

  std::string table_name;
};

}

/// @endcond

}

#endif /* TOKEN_HPP */
