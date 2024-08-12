#include <catch2/catch_test_macros.hpp>

#include "matador/sql/connection_info.hpp"
#include "matador/sql/connection_pool.hpp"
#include "matador/sql/session.hpp"
#include "matador/sql/statement_cache.hpp"

#include "connection.hpp"

using namespace matador;

class StatementCacheFixture
{
public:
  StatementCacheFixture()
  : pool(matador::test::connection::dns, 4), ses(pool)
  {}
  ~StatementCacheFixture() = default;

protected:
  matador::sql::connection_pool<matador::sql::connection> pool;
  matador::sql::session ses;
};

TEST_CASE_METHOD(StatementCacheFixture, "Acquire prepared statement", "[statement cache]") {
  sql::statement_cache cache;

  auto conn = pool.acquire();

  std::string sql = R"(SELECT * FROM person WHERE name = 'george')";
//  auto &stmt = cache.acquire(sql, *conn);
}