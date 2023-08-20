#include "SqlLoggerTest.hpp"

#include "matador/utils/os.hpp"

#include "matador/logger/log_manager.hpp"

#include "matador/sql/sql_logger.hpp"
#include "matador/sql/basic_sql_logger.hpp"

#include "../logger/file.hpp"

using namespace matador;

SqlLoggerTest::SqlLoggerTest()
  : unit_test("sql_logger", "sql logger test unit")
{
  add_test("null_logger", [this] { test_null_logger(); }, "test null logger");
  add_test("sql_logger", [this] { test_sql_logger(); }, "test sql logger");
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
  auto path = matador::os::build_path("log", "sql.log");
  // Redirect stdout
  {
    filehelper::std_stream_switcher stdout_switcher(stdout, "stdout.txt");
    sql_logger sqllogger;

    UNIT_ASSERT_FALSE(matador::os::exists(path));

    sqllogger.on_connect();
    sqllogger.on_close();
    sqllogger.on_execute("SELECT * FROM person");
    sqllogger.on_prepare("SELECT * FROM person WHERE id=5");
  }

  matador::os::remove("stdout.txt");

  UNIT_ASSERT_FALSE(matador::os::exists("stdout.txt"));

  matador::os::remove(path);

  UNIT_ASSERT_FALSE(matador::os::exists(path));

  matador::log_manager::instance().clear();
}
