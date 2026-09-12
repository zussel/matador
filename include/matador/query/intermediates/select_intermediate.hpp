#ifndef QUERY_SELECT_INTERMEDIATE_HPP
#define QUERY_SELECT_INTERMEDIATE_HPP

#include "matador/query/intermediates/intermediate.hpp"
#include "matador/query/intermediates/from_intermediate.hpp"

#include "matador/query/column.hpp"

#include <stdexcept>
#include <vector>

namespace matador::query {

class from_intermediate;

class select_intermediate : public intermediate
{
public:
  explicit select_intermediate(const std::vector<column>& columns);

  fetchable_query nextval(const std::string& sequence_name);
  fetchable_query currval(const std::string& sequence_name);

  template<typename... Tables>
  from_intermediate from(const Tables&... tables) {
    if constexpr (sizeof...(tables) == 0) {
      throw std::invalid_argument("SELECT requires at least one table in FROM");
    }
    std::vector<table> v { tables... };
    return from(v);
  }

private:
  from_intermediate from(const std::vector<table>& tables);
};

}

#endif //QUERY_SELECT_INTERMEDIATE_HPP
