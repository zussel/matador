#ifndef MATADOR_QUERY_FIXTURE_HPP
#define MATADOR_QUERY_FIXTURE_HPP

#include "matador/sql/connection.hpp"
#include "matador/sql/schema.hpp"

#include "connection.hpp"

#include <set>

namespace matador::test {

class QueryFixture {
public:
  QueryFixture();
  ~QueryFixture();

protected:
  matador::sql::connection db;
  matador::sql::schema schema;
  std::set <std::string> tables_to_drop;

private:
  void drop_table_if_exists(const std::string &table_name);
};

}

#endif //MATADOR_QUERY_FIXTURE_HPP
