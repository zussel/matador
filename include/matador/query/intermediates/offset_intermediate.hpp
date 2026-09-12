#ifndef QUERY_OFFSET_INTERMEDIATE_HPP
#define QUERY_OFFSET_INTERMEDIATE_HPP

#include "matador/query/intermediates/fetchable_query.hpp"

namespace matador::query {
class limit_intermediate;

class offset_intermediate : public fetchable_query {
public:
  using fetchable_query::fetchable_query;

  limit_intermediate limit(size_t limit);
};
}

#endif //QUERY_OFFSET_INTERMEDIATE_HPP
