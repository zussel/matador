#ifndef MATADOR_QUERY_FIXTURE_HPP
#define MATADOR_QUERY_FIXTURE_HPP

#include "matador/sql/connection.hpp"
#include "matador/sql/schema.hpp"

#include "connection.hpp"

#include <stack>

namespace matador::test {

class QueryFixture {
public:
  QueryFixture();
  ~QueryFixture();

  void check_table_exists(const std::string &table_name) const;
  void check_table_not_exists(const std::string &table_name) const;

protected:
  sql::connection db;
  sql::schema schema;
  std::stack <std::string> tables_to_drop;

private:
  void drop_table_if_exists(const std::string &table_name) const;
};

}

#endif //MATADOR_QUERY_FIXTURE_HPP
