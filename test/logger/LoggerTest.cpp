#include "LoggerTest.hpp"
#include "file.hpp"

#include "matador/logger/file_sink.hpp"
#include "matador/logger/log_manager.hpp"

#include "matador/utils/os.hpp"

#include <cstdio>
#include <fstream>
#include <regex>
#include <sstream>

#ifdef _MSC_VER
#include <io.h>
#else
#endif

LoggerTest::LoggerTest()
  : unit_test("logger", "logger test")
{
  add_test("log_level_range", [this] { test_log_level_range(); }, "logger log level range test");
  add_test("file_sink", [this] { test_file_sink(); }, "logger file sink test");
  add_test("rotating_file_sink", [this] { test_rotating_file_sink(); }, "logger rotating file sink test");
  add_test("logger", [this] { test_logger(); }, "logger test");
  add_test("logging", [this] { test_logging(); }, "logger logging test");
  add_test("stdout", [this] { test_stdout(); }, "logger stdout logging test");
  add_test("stderr", [this] { test_stderr(); }, "logger stderr logging test");
  add_test("level", [this] { test_log_level(); }, "print log level test");
}

void LoggerTest::test_log_level_range()
{
  UNIT_ASSERT_EQUAL(matador::log_level::LVL_INFO, matador::log_manager::min_default_log_level());
  UNIT_ASSERT_EQUAL(matador::log_level::LVL_FATAL, matador::log_manager::max_default_log_level());

  matador::default_min_log_level(matador::log_level::LVL_DEBUG);
  matador::default_max_log_level(matador::log_level::LVL_ERROR);

  UNIT_ASSERT_EQUAL(matador::log_level::LVL_DEBUG, matador::log_manager::min_default_log_level());
  UNIT_ASSERT_EQUAL(matador::log_level::LVL_ERROR, matador::log_manager::max_default_log_level());

  matador::log_level_range llr;
  llr.min_level = matador::log_level::LVL_DEBUG;
  llr.max_level = matador::log_level::LVL_TRACE;
  matador::log_domain ld("test", llr);

  UNIT_ASSERT_EQUAL(matador::log_level::LVL_DEBUG, ld.min_log_level());
  UNIT_ASSERT_EQUAL(matador::log_level::LVL_TRACE, ld.max_log_level());

  ld.min_log_level(matador::log_level::LVL_INFO);
  ld.max_log_level(matador::log_level::LVL_ERROR);

  UNIT_ASSERT_EQUAL(matador::log_level::LVL_INFO, ld.min_log_level());
  UNIT_ASSERT_EQUAL(matador::log_level::LVL_ERROR, ld.max_log_level());
}

void LoggerTest::test_file_sink()
{
  matador::file_sink test("test.txt");

  UNIT_ASSERT_TRUE(matador::os::exists("test.txt"));

//  UNIT_ASSERT_EQUAL("test.txt", test.path());

  test.close();

  if (::remove("test.txt") == -1) {
#ifdef _MSC_VER
    char buf[1024];
    strerror_s(buf, 1024, errno);
    UNIT_FAIL(buf);
#else
    UNIT_FAIL(strerror(errno));
#endif
  }

  UNIT_ASSERT_FALSE(matador::os::exists("test.txt"));

  {
    auto path = matador::os::build_path("mylog", "test.txt");
    matador::file_sink test2(path);

    UNIT_ASSERT_EQUAL("mylog", test2.path());

    matador::os::chdir("mylog");

    UNIT_ASSERT_TRUE(matador::os::exists("test.txt"));

  }
  if (::remove("test.txt") == -1) {
#ifdef _MSC_VER
    char buf[1024];
    strerror_s(buf, 1024, errno);
    UNIT_FAIL(buf);
#else
    UNIT_FAIL(strerror(errno));
#endif
  }

  UNIT_ASSERT_FALSE(matador::os::exists("test.txt"));

  matador::os::chdir("..");

  matador::os::rmdir("mylog");
}

void LoggerTest::test_rotating_file_sink()
{
  auto dir = matador::os::build_path("my", "log");
  auto path = matador::os::build_path(dir, "log.txt");

  auto logsink = matador::create_rotating_file_sink(path, 30, 3);

  matador::os::chdir(dir);

  UNIT_ASSERT_TRUE(matador::os::is_readable("log.txt"));
  UNIT_ASSERT_TRUE(matador::os::is_writable("log.txt"));

  std::string line = "hello world and first line\n";

  logsink->write(line.c_str(), line.size());

  matador::os::chdir("my");
  matador::os::chdir("log");

  UNIT_ASSERT_FALSE(matador::os::exists("log.1.txt"));

  logsink->write(line.c_str(), line.size());

  UNIT_ASSERT_TRUE(matador::os::exists("log.1.txt"));

  logsink->close();

  matador::os::remove("log.txt");
  matador::os::remove("log.1.txt");

  matador::os::chdir("..");
  matador::os::chdir("..");

  matador::os::rmpath(matador::os::build_path("my", "log"));
}

void LoggerTest::test_logger()
{
  auto logger = matador::create_logger("test");

  UNIT_ASSERT_EQUAL("test", logger.source());
  UNIT_ASSERT_EQUAL("default", logger.domain());

  auto logsink = matador::create_file_sink("log.txt");

  UNIT_ASSERT_TRUE(matador::os::is_readable("log.txt"));
  UNIT_ASSERT_TRUE(matador::os::is_writable("log.txt"));

  logsink->close();

  if (::remove("log.txt") == -1) {
#ifdef _MSC_VER
    char buf[1024];
    strerror_s(buf, 1024, errno);
    UNIT_FAIL(buf);
#else
    UNIT_FAIL(strerror(errno));
#endif
  }

  UNIT_ASSERT_FALSE(matador::os::exists("log.txt"));

  logger = matador::create_logger("net", "system");

  UNIT_ASSERT_EQUAL("net", logger.source());
  UNIT_ASSERT_EQUAL("system", logger.domain());

  logsink = matador::create_file_sink("net.txt");

  UNIT_ASSERT_TRUE(matador::os::is_readable("net.txt"));
  UNIT_ASSERT_TRUE(matador::os::is_writable("net.txt"));

  logsink->close();

  matador::os::remove("net.txt");

  UNIT_ASSERT_FALSE(matador::os::exists("net.txt"));

  matador::log_manager::instance().clear();
}

void LoggerTest::test_logging()
{
  matador::domain_min_log_level("default", matador::log_level::LVL_FATAL);
  matador::domain_max_log_level("default", matador::log_level::LVL_TRACE);

  auto logger = matador::create_logger("test");

  UNIT_ASSERT_EQUAL("test", logger.source());
  UNIT_ASSERT_EQUAL("default", logger.domain());

  auto logsink = matador::create_file_sink("log.txt");

  UNIT_ASSERT_TRUE(matador::os::is_readable("log.txt"));
  UNIT_ASSERT_TRUE(matador::os::is_writable("log.txt"));

  matador::add_log_sink(logsink);

  logger.info("information");
  logger.info("information %s", "important");
  logger.warn("warning");
  logger.warn("warning %s", "important");
  logger.debug("debugging");
  logger.debug("debugging %s", "important");
  logger.trace("tracing something");
  logger.trace("tracing something %s", "important");
  logger.error("big error");
  logger.error("big error %s", "important");
  matador::log(matador::log_level::LVL_ERROR, "test", "global log test %d", 4711);

  logsink->close();

  validate_log_file_line("log.txt", 0, "INFO", "test", "information");
  validate_log_file_line("log.txt", 1, "INFO", "test", "information important");
  validate_log_file_line("log.txt", 2, "WARN", "test", "warning");
  validate_log_file_line("log.txt", 3, "WARN", "test", "warning important");
  validate_log_file_line("log.txt", 4, "DEBUG", "test", "debugging");
  validate_log_file_line("log.txt", 5, "DEBUG", "test", "debugging important");
  validate_log_file_line("log.txt", 6, "TRACE", "test", "tracing something");
  validate_log_file_line("log.txt", 7, "TRACE", "test", "tracing something important");
  validate_log_file_line("log.txt", 8, "ERROR", "test", "big error");
  validate_log_file_line("log.txt", 9, "ERROR", "test", "big error important");
  validate_log_file_line("log.txt", 10, "ERROR", "test", "global log test 4711");

  matador::os::remove("log.txt");

  UNIT_ASSERT_FALSE(matador::os::exists("log.txt"));

  matador::log_manager::instance().clear();
}

void LoggerTest::test_stdout()
{
  auto logger = matador::create_logger("test");

  UNIT_ASSERT_EQUAL("test", logger.source());
  UNIT_ASSERT_EQUAL("default", logger.domain());

  auto logsink = matador::create_stdout_sink();

  // Redirect stdout
  {
    filehelper::std_stream_switcher stdout_switcher(stdout, "stdout.txt");
    matador::add_log_sink(logsink);
    logger.info("information");
  }

  logsink->close();

  validate_log_file_line("stdout.txt", 0, "INFO", "test", "information");

  matador::os::remove("stdout.txt");

  UNIT_ASSERT_FALSE(matador::os::exists("stdout.txt"));

  matador::log_manager::instance().clear();
}

void LoggerTest::test_stderr()
{
  auto logger = matador::create_logger("test");

  UNIT_ASSERT_EQUAL("test", logger.source());
  UNIT_ASSERT_EQUAL("default", logger.domain());

  auto logsink = matador::create_stderr_sink();

  // Redirect stdout
  {
    filehelper::std_stream_switcher stdout_switcher(stderr, "stderr.txt");
    matador::add_log_sink(logsink);
    logger.info("information");
  }

  logsink->close();

  validate_log_file_line("stderr.txt", 0, "INFO", "test", "information");

  matador::os::remove("stderr.txt");

  UNIT_ASSERT_FALSE(matador::os::exists("stderr.txt"));

  matador::log_manager::instance().clear();
}

void LoggerTest::test_log_level()
{
  std::stringstream out;

  out << matador::log_level::LVL_ERROR;

  UNIT_ASSERT_EQUAL("ERROR", out.str().c_str());

  out.str("");
  out.clear();
  out << matador::log_level::LVL_DEBUG;

  UNIT_ASSERT_EQUAL("DEBUG", out.str().c_str());

  out.str("");
  out.clear();
  out << matador::log_level::LVL_INFO;

  UNIT_ASSERT_EQUAL("INFO", out.str().c_str());

  out.str("");
  out.clear();
  out << matador::log_level::LVL_FATAL;

  UNIT_ASSERT_EQUAL("FATAL", out.str().c_str());

  out.str("");
  out.clear();
  out << matador::log_level::LVL_TRACE;

  UNIT_ASSERT_EQUAL("TRACE", out.str().c_str());

  out.str("");
  out.clear();
  out << matador::log_level::LVL_WARN;

  UNIT_ASSERT_EQUAL("WARN", out.str().c_str());

  out.str("");
  out.clear();
  out << matador::log_level::LVL_ALL;

  UNIT_ASSERT_EQUAL("ALL", out.str().c_str());
}

void LoggerTest::validate_log_file_line(const std::string &filename, int line_index, const std::string &level, const std::string &src, const std::string &msg)
{
  std::ifstream logfile(filename);

  UNIT_ASSERT_TRUE(logfile.good());

  std::string line;
  int line_count = 0;
  do {
    if (!std::getline(logfile, line).good()) {
      UNIT_FAIL("couldn't find line");
    }
  } while (line_count++ < line_index);

  const std::regex logline_regex(R"(^\d{4}-\d\d-\d\d \d\d:\d\d:\d\d\.\d{3}.*\[([A-Z]+)\s*\] \[(.*)\]: (.*)$)");
  std::smatch match;

  UNIT_ASSERT_TRUE(std::regex_match(line, match, logline_regex));
  UNIT_ASSERT_EQUAL(4UL, match.size());

  UNIT_ASSERT_EQUAL(level, match[1].str());
  UNIT_ASSERT_EQUAL(src, match[2].str());
  UNIT_ASSERT_EQUAL(msg, match[3].str());
}