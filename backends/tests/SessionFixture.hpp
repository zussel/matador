#ifndef MATADOR_SESSION_FIXTURE_HPP
#define MATADOR_SESSION_FIXTURE_HPP

#include "matador/sql/session.hpp"

#include "connection.hpp"

#include <set>

namespace matador::test {

class SessionFixture {
public:
  SessionFixture();
  ~SessionFixture();

protected:
  matador::sql::connection_pool<matador::sql::connection> pool;
  matador::sql::session ses;
  std::set <std::string> tables_to_drop;

private:
  void drop_table_if_exists(const std::string &table_name);

};

}

#endif //MATADOR_SESSION_FIXTURE_HPP
