#ifndef MATADOR_SESSION_FIXTURE_HPP
#define MATADOR_SESSION_FIXTURE_HPP

#include "matador/utils/message_bus.hpp"

#include "matador/query/runtime/connection.hpp"
#include "matador/query/runtime/schema_manager.hpp"

#include "matador/query/schema/schema.hpp"

namespace matador::test {

class SessionFixture {
public:
  SessionFixture();
  ~SessionFixture();

protected:
  utils::message_bus bus;
  query::connection db;

  query::schema schema;
  query::schema_manager manager;

private:
  void drop_table_if_exists(const std::string &table_name) const;

};

}

#endif //MATADOR_SESSION_FIXTURE_HPP
