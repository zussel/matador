#ifndef MATADOR_QUERY_VALUES_INTERMEDIATE_H
#define MATADOR_QUERY_VALUES_INTERMEDIATE_H

#include "matador/query/intermediates/executable_query.hpp"
#include "matador/query/intermediates/fetchable_query.hpp"

namespace matador::query {
class values_intermediate : public executable_query {
public:
  using executable_query::executable_query;

  template<typename... Columns>
  fetchable_query returning(const Columns&... columns) const {
    return returning(std::vector<column>{columns...});
  }

private:
  fetchable_query returning(const std::vector<column>& columns) const;
};
}

#endif //MATADOR_QUERY_VALUES_INTERMEDIATE_H