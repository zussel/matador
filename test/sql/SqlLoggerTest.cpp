#include <matador/sql/sql_logger.hpp>
#include "SqlLoggerTest.hpp"

#include "matador/sql/basic_sql_logger.hpp"

using namespace matador;

SqlLoggerTest::SqlLoggerTest()
  : unit_test("sql_logger", "sql logger test unit")
{
  add_test("null_logger", std::bind(&SqlLoggerTest::test_null_logger, this), "test null logger");
  add_test("sql_logger", std::bind(&SqlLoggerTest::test_sql_logger, this), "test sql logger");

}

void SqlLoggerTest::test_null_logger()
{
  null_sql_logger null_logger;

  null_logger.on_connect();
  null_logger.on_close();
  null_logger.on_execute("");
  null_logger.on_prepare("");

  UNIT_ASSERT_TRUE(true);
}

void SqlLoggerTest::test_sql_logger()
{
  sql_logger sqlLogger;

}
