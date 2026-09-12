#ifndef JOIN_DATA_HPP
#define JOIN_DATA_HPP

#include "matador/query/criteria/abstract_criteria.hpp"

#include "matador/query/table.hpp"

#include <memory>

namespace matador::query {

struct join_data {
  const table* join_table{nullptr};
  std::unique_ptr<abstract_criteria> condition;
};

}
#endif //JOIN_DATA_HPP
