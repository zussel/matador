#ifndef QUERY_CREATE_INTERMEDIATE_HPP
#define QUERY_CREATE_INTERMEDIATE_HPP

#include "matador/query/intermediates/executable_query.hpp"

#include "matador/query/model/column.hpp"
#include "matador/query/model/constraint.hpp"

#include <list>

namespace matador::query {
class table;
class create_table_columns_intermediate : public executable_query {
public:
  using executable_query::executable_query;

  [[nodiscard]] executable_query constraints(std::initializer_list<constraint> constraints) const;
  [[nodiscard]] executable_query constraints(const std::list<constraint> &constraints) const;
};

class create_table_intermediate : public intermediate {
public:
  using intermediate::intermediate;

  [[nodiscard]] create_table_columns_intermediate columns(std::initializer_list<column> cols) const;
  [[nodiscard]] create_table_columns_intermediate columns(const std::list<column> &cols) const;
  [[nodiscard]] create_table_columns_intermediate columns(const std::vector<column> &cols) const;
};

class query_create_sequence_intermediate : public executable_query {
public:
  using executable_query::executable_query;
};

class create_intermediate : public intermediate {
public:
  create_intermediate();

  [[nodiscard]] create_table_intermediate table(const std::string &table_name) const;
  [[nodiscard]] create_table_intermediate table(const class table &tab) const;
  [[nodiscard]] query_create_sequence_intermediate sequence(const std::string &sequence_name) const;
  [[nodiscard]] executable_query schema(const std::string &schema_name) const;
};

}


#endif //QUERY_CREATE_INTERMEDIATE_HPP
