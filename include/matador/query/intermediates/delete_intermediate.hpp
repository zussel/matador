#ifndef QUERY_DELETE_INTERMEDIATE_HPP
#define QUERY_DELETE_INTERMEDIATE_HPP

#include "matador/query/intermediates/intermediate.hpp"
#include "matador/query/intermediates/delete_from_intermediate.hpp"

#include "matador/query/table.hpp"

namespace matador::query {
class delete_intermediate : public intermediate {
public:
  delete_intermediate();

  /**
   * Specifies the table to delete from.
   *
   * @code{.cpp}
   * auto query = remove().from(customers).where("id"_col == 42);
   * @endcode
   */
  delete_from_intermediate from(const table &tab) const;
};
}

#endif //QUERY_DELETE_INTERMEDIATE_HPP
