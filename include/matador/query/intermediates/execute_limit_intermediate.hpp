#ifndef QUERY_EXECUTE_LIMIT_INTERMEDIATE_HPP
#define QUERY_EXECUTE_LIMIT_INTERMEDIATE_HPP

#include "matador/query/intermediates/executable_query.hpp"

namespace matador::query {
class execute_offset_intermediate;

class execute_limit_intermediate : public executable_query {
public:
  using executable_query::executable_query;

  execute_offset_intermediate offset(size_t offset) const;
};
}

#endif //QUERY_EXECUTE_LIMIT_INTERMEDIATE_HPP
