#ifndef QUERY_CREATE_INTERMEDIATE_HPP
#define QUERY_CREATE_INTERMEDIATE_HPP

#include "matador/query/intermediates/executable_query.hpp"

#include "matador/query/constraint.hpp"
#include "matador/query/column.hpp"

#include <list>

namespace matador::query {
class table;
class create_table_columns_intermediate : public executable_query {
public:
  using executable_query::executable_query;

  executable_query constraints(std::initializer_list<constraint> constraints);
  executable_query constraints(const std::list<constraint> &constraints);
};

class create_table_intermediate : public intermediate {
public:
  using intermediate::intermediate;

  create_table_columns_intermediate columns(std::initializer_list<column> cols);
  create_table_columns_intermediate columns(const std::list<column> &cols);
  create_table_columns_intermediate columns(const std::vector<column> &cols);
};

class query_create_sequence_intermediate : public executable_query {
public:
  using executable_query::executable_query;
};

class create_intermediate : public intermediate {
public:
  create_intermediate();

  create_table_intermediate table(const table &tab);
  query_create_sequence_intermediate sequence(const std::string &sequence_name);
  executable_query schema(const std::string &schema_name);
};

}


#endif //QUERY_CREATE_INTERMEDIATE_HPP
