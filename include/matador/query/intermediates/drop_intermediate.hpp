#ifndef QUERY_DROP_INTERMEDIATE_HPP
#define QUERY_DROP_INTERMEDIATE_HPP

#include "matador/query/intermediates/executable_query.hpp"

namespace matador::query {
class table;

class drop_intermediate : public intermediate {
public:
  drop_intermediate();

  /** Drops a table without changing other states derived from this drop state. */
  executable_query table(const table &tab) const;
  /** Drops a sequence without changing other states derived from this drop state. */
  executable_query sequence(const std::string &sequence_name) const;
  /** Drops a schema without changing other states derived from this drop state. */
  executable_query schema(const std::string &schema_name) const;
};

}

#endif //QUERY_DROP_INTERMEDIATE_HPP
