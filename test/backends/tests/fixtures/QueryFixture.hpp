#ifndef MATADOR_QUERY_FIXTURE_HPP
#define MATADOR_QUERY_FIXTURE_HPP

#include "matador/query/schema/schema.hpp"

#include "matador/query/runtime/connection.hpp"
#include "matador/query/runtime/schema_manager.hpp"

#include <stack>

namespace matador::test {

class QueryFixture {
public:
  QueryFixture();
  ~QueryFixture();

  void check_table_exists(const std::string &table_name) const;
  void check_table_not_exists(const std::string &table_name) const;

protected:
  query::connection db;
  std::stack <std::string> tables_to_drop;
  query::schema repo;
  query::schema_manager manager;

private:
  void drop_table_if_exists(const std::string &table_name) const;
};

}

#endif //MATADOR_QUERY_FIXTURE_HPP
