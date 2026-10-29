#ifndef QUERY_INSERT_INTERMEDIATE_HPP
#define QUERY_INSERT_INTERMEDIATE_HPP

#include "matador/query/intermediates/intermediate.hpp"
#include "matador/query/intermediates/into_intermediate.hpp"
// #include "matador/query/generator.hpp"
#include "matador/query/model/column.hpp"

namespace matador::query {
// class schema;
class table;

class insert_intermediate : public intermediate {
public:
  insert_intermediate();

  // template<class Type>
  // query_into_intermediate into(const table &tab, const schema &scm) {
    // return into(tab, generator::columns<Type>(scm));
  // }
  [[nodiscard]] into_intermediate into(const table &tab, std::initializer_list<column> columns) const;
  [[nodiscard]] into_intermediate into(const table &tab, const std::vector<column> &columns) const;
  [[nodiscard]] into_intermediate into(const std::string &table_name, const std::vector<std::string> &column_names) const;
  [[nodiscard]] into_intermediate into(const table &tab, const std::vector<std::string> &column_names) const;
  [[nodiscard]] into_intermediate into(const table &tab) const;
};

}

#endif //QUERY_INSERT_INTERMEDIATE_HPP
