#ifndef MATADOR_SESSION_FIXTURE_HPP
#define MATADOR_SESSION_FIXTURE_HPP

#include "matador/sql/session.hpp"

#include "connection.hpp"

#include <stack>

namespace matador::test {

class SessionFixture {
public:
  SessionFixture();
  ~SessionFixture();

protected:
  sql::connection_pool<sql::connection> pool;
  sql::session ses;
  std::stack <std::string> tables_to_drop;

private:
  void drop_table_if_exists(const std::string &table_name) const;

};

}

#endif //MATADOR_SESSION_FIXTURE_HPP
