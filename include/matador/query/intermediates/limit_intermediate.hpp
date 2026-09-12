#ifndef QUERY_LIMIT_INTERMEDIATE_HPP
#define QUERY_LIMIT_INTERMEDIATE_HPP

#include "matador/query/intermediates/fetchable_query.hpp"

namespace matador::query {
class offset_intermediate;

class limit_intermediate : public fetchable_query {
public:
  using fetchable_query::fetchable_query;

  offset_intermediate offset(size_t offset);
};
}

#endif //QUERY_LIMIT_INTERMEDIATE_HPP
