#ifndef QUERY_QUERY_PART_HPP
#define QUERY_QUERY_PART_HPP

#include "matador/sql/dialect.hpp"

namespace matador::sql {

class query_part_visitor;

class query_part
{
protected:
  explicit query_part(sql::dialect::token_t token);

public:
  virtual ~query_part() = default;
  virtual void accept(query_part_visitor &visitor) = 0;

  [[nodiscard]] dialect::token_t token() const;

protected:
  sql::dialect::token_t token_;
};

}
#endif //QUERY_QUERY_PART_HPP
