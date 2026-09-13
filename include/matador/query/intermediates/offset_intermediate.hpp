#ifndef QUERY_OFFSET_INTERMEDIATE_HPP
#define QUERY_OFFSET_INTERMEDIATE_HPP

#include "matador/query/intermediates/fetchable_query.hpp"

namespace matador::query {
class limit_intermediate;

class offset_intermediate : public fetchable_query {
public:
  using fetchable_query::fetchable_query;

  /** Appends a LIMIT clause and transitions to the fetchable limit state. */
  limit_intermediate limit(size_t limit) const;
};
}

#endif //QUERY_OFFSET_INTERMEDIATE_HPP
