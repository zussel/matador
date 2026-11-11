#ifndef MATADOR_SESSION_FIXTURE_HPP
#define MATADOR_SESSION_FIXTURE_HPP

#include "matador/query/runtime/session.hpp"

#include "matador/query/schema/schema.hpp"

#include "matador/utils/message_bus.hpp"

namespace matador::test {
class SessionFixture {
public:
  SessionFixture();
  ~SessionFixture();

protected:
  query::session_context make_session_context(size_t connection_count);

protected:
  utils::message_bus bus;
  std::unique_ptr<query::connection> db;
  query::schema scm;
};
}

#endif // MATADOR_SESSION_FIXTURE_HPP
