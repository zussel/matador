#ifndef MATADOR_QUERY_ALTER_INTERMEDIATE_HPP
#define MATADOR_QUERY_ALTER_INTERMEDIATE_HPP

#include "matador/query/intermediates/alter_table_intermediate.hpp"
#include "matador/query/intermediates/intermediate.hpp"

namespace matador::query {
class alter_intermediate : public intermediate {
public:
  alter_intermediate();

  [[nodiscard]] alter_table_intermediate table(const table &tab) const;
};
}
#endif //MATADOR_QUERY_ALTER_INTERMEDIATE_HPP