#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "matador/sql/types.hpp"

#include <memory>
#include <sstream>
#include <vector>
#include <iterator>

namespace matador {

class token_visitor;

namespace detail {

/// @cond MATADOR_DEV
struct token
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
    INTO,
    WHERE,
    AND,
    OR,
    LIKE,
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

struct table_name_token : public token
{
  table_name_token(t_token tok, std::string table_name);

  std::string table_name;
};

}

/// @endcond

}

#endif /* TOKEN_HPP */
