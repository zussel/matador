#include "LoggerTest.hpp"

#include "matador/logger/file_sink.hpp"
#include "matador/logger/log_manager.hpp"

#include <cstdio>
#include <fstream>
#include <regex>

#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

namespace filehelper {

#ifdef _MSC_VER

enum access_type
{
  EXISTS = 0,
  WRITABLE = 2,
  READABLE = 4,
  READABLE_WRITABLE = 6
};

bool access(const char *path, access_type mode)
{
  return _access(path, mode) == 0;
}
#else

enum access_type
{
  EXISTS = 0,
  EXECUTABLE = 1,
  WRITABLE = 2,
  READABLE = 4,
  READABLE_WRITABLE = 6
};

bool access(const char *path, access_type mode)
{
  return ::access(path, mode) == 0;
}
#endif

bool file_is_readable(const char *path)
{
  return access(path, READABLE);
}

bool file_is_writable(const char *path)
{
  return access(path, WRITABLE);
}

bool file_exists(const char *path)
{
  return access(path, EXISTS);
}

bool validate_log_file_line(const std::string &filename, int line_index, const std::string &level, const std::string &src, const std::string &msg);

}

LoggerTest::LoggerTest()
  : unit_test("logger", "logger test")
{
  add_test("logger", std::bind(&LoggerTest::test_logger, this), "logger test");
  add_test("logging", std::bind(&LoggerTest::test_logging, this), "logger logging test");
  add_test("stdout", std::bind(&LoggerTest::test_stdout, this), "logger stdout logging test");
  add_test("stderr", std::bind(&LoggerTest::test_stderr, this), "logger stderr logging test");
}

void LoggerTest::test_logger()
{
  auto logger = matador::create_logger("test");

  UNIT_ASSERT_EQUAL("test", logger.source());
  UNIT_ASSERT_EQUAL("default", logger.domain());

  auto logsink = std::make_shared<matador::file_sink>("log.txt");

  UNIT_ASSERT_TRUE(filehelper::file_is_readable("log.txt"));
  UNIT_ASSERT_TRUE(filehelper::file_is_writable("log.txt"));

  logsink->close();

  if (::remove("log.txt") == -1) {
    UNIT_FAIL(strerror(errno));
  }

  UNIT_ASSERT_FALSE(filehelper::file_exists("log.txt"));

  logger = matador::create_logger("net", "system");

  UNIT_ASSERT_EQUAL("net", logger.source());
  UNIT_ASSERT_EQUAL("system", logger.domain());

  logsink = std::make_shared<matador::file_sink>("net.txt");

  UNIT_ASSERT_TRUE(filehelper::file_is_readable("net.txt"));
  UNIT_ASSERT_TRUE(filehelper::file_is_writable("net.txt"));

  logsink->close();

  ::remove("net.txt");

  UNIT_ASSERT_FALSE(filehelper::file_exists("net.txt"));

  matador::log_manager::instance().clear();
}

void LoggerTest::test_logging()
{
  auto logger = matador::create_logger("test");

  UNIT_ASSERT_EQUAL("test", logger.source());
  UNIT_ASSERT_EQUAL("default", logger.domain());

  auto logsink = std::make_shared<matador::file_sink>("log.txt");

  UNIT_ASSERT_TRUE(filehelper::file_is_readable("log.txt"));
  UNIT_ASSERT_TRUE(filehelper::file_is_writable("log.txt"));

  matador::add_log_sink(logsink);

  logger.info("information");
  logger.warn("warning");
  logger.debug("debugging");
  logger.trace("tracing something");
  logger.error("big error");

  logsink->close();

  UNIT_ASSERT_TRUE(filehelper::validate_log_file_line("log.txt", 0, "INFO", "test", "information"));
  UNIT_ASSERT_TRUE(filehelper::validate_log_file_line("log.txt", 1, "WARN", "test", "warning"));
  UNIT_ASSERT_TRUE(filehelper::validate_log_file_line("log.txt", 2, "DEBUG", "test", "debugging"));
  UNIT_ASSERT_TRUE(filehelper::validate_log_file_line("log.txt", 3, "TRACE", "test", "tracing something"));
  UNIT_ASSERT_TRUE(filehelper::validate_log_file_line("log.txt", 4, "ERROR", "test", "big error"));


  ::remove("log.txt");

  UNIT_ASSERT_FALSE(filehelper::file_exists("log.txt"));

  matador::log_manager::instance().clear();
}


class std_stream_switcher
{
public:
  explicit std_stream_switcher(FILE *str, const char* redirect)
    : stream(str)
  {
    fflush(stream);
    fgetpos(stream, &pos);
    fd = dup(fileno(stream));
    freopen(redirect, "w+", stream);
  }

  ~std_stream_switcher()
  {
    fflush(stream);
    dup2(fd, fileno(stream));
    close(fd);
    clearerr(stream);
    fsetpos(stream, &pos);
  }
private:
  FILE *stream = nullptr;
  int fd = 0;
  fpos_t pos = {};
};

void LoggerTest::test_stdout()
{
  auto logger = matador::create_logger("test");

  UNIT_ASSERT_EQUAL("test", logger.source());
  UNIT_ASSERT_EQUAL("default", logger.domain());

  auto logsink = std::make_shared<matador::stdout_sink>();

  // Redirect stdout
  {
    std_stream_switcher stdout_switcher(stdout, "stdout.txt");
    matador::add_log_sink(logsink);
    logger.info("information");
  }

  UNIT_ASSERT_TRUE(filehelper::validate_log_file_line("stdout.txt", 0, "INFO", "test", "information"));

  ::remove("stdout.txt");

  UNIT_ASSERT_FALSE(filehelper::file_exists("stdout.txt"));

  matador::log_manager::instance().clear();
}

void LoggerTest::test_stderr()
{
  auto logger = matador::create_logger("test");

  UNIT_ASSERT_EQUAL("test", logger.source());
  UNIT_ASSERT_EQUAL("default", logger.domain());

  auto logsink = std::make_shared<matador::stderr_sink>();

  // Redirect stdout
  {
    std_stream_switcher stdout_switcher(stderr, "stderr.txt");
    matador::add_log_sink(logsink);
    logger.info("information");
  }

  UNIT_ASSERT_TRUE(filehelper::validate_log_file_line("stderr.txt", 0, "INFO", "test", "information"));

  ::remove("stderr.txt");

  UNIT_ASSERT_FALSE(filehelper::file_exists("stderr.txt"));

  matador::log_manager::instance().clear();
}

namespace filehelper {
bool validate_log_file_line(const std::string &filename, int line_index, const std::string &level, const std::string &src, const std::string &msg)
{
  std::ifstream logfile(filename);

  if (!logfile.good()) {
    return false;
  }

  std::string line;
  int line_count = 0;
  do {
    if (!std::getline(logfile, line).good()) {
      return false;
    }
  } while (line_count++ < line_index);

  const std::regex logline_regex(R"(^\d{4}-\d\d-\d\d \d\d:\d\d:\d\d\.\d{3}.*\[([A-Z]+)\s*\]\[(.*)\]: (.*)$)");
  std::smatch match;

  if (!std::regex_match(line, match, logline_regex)) {
    return false;
  }
  if (match.size() != 4) {
    return false;
  }

  if (match[1].str() != level) {
    return false;
  }

  if (match[2].str() != src) {
    return false;
  }

  return match[3].str() == msg;
}
}