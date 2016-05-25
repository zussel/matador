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

#include "sql/types.hpp"

#include <memory>
#include <sstream>
#include <vector>
#include <iterator>

namespace oos {

class basic_dialect;

namespace detail {

/// @cond OOS_DEV
struct token
{
  enum t_token
  {
    CREATE_TABLE = 0,
    DROP,
    DELETE,
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
    ROLLBACK,
    NONE
  };

  explicit token(t_token tok);
  virtual ~token() {}

  virtual std::string compile(basic_dialect &d) const = 0;

  t_token type;
};

}

/// @endcond

}

#endif /* TOKEN_HPP */
