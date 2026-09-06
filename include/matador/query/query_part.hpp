#ifndef QUERY_QUERY_PART_HPP
#define QUERY_QUERY_PART_HPP

#include "matador/query/dialect_token.hpp"

namespace matador::query {

class query_part_visitor;

class query_part {
protected:
  explicit query_part(dialect_token token);

public:
  virtual ~query_part() = default;
  virtual void accept(query_part_visitor &visitor) = 0;

  [[nodiscard]] dialect_token token() const;

protected:
  dialect_token token_;
};

}
#endif //QUERY_QUERY_PART_HPP
