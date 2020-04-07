#include "matador/utils/time.hpp"
#include "matador/utils/memory.hpp"

#include <string>
#include <map>
#include <list>
#include <memory>
#include <ctime>

namespace matador {

class log_manager;

class logger
{
public:
  enum level {
    LVL_INFO, LVL_WARN, LVL_DEBUG, LVL_ERROR, LVL_TRACE
  };

  static constexpr const char* TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S";

  static std::map<level, std::string> level_strings;

  explicit logger(const char *source, log_manager &manager)
    : source_(source)
    , manager_(manager)
  {}

  template < typename ... ARGS >
  void info(const std::string &what, ARGS const & ... args)
  {
    info(what.c_str(), args...);
  }

  template < typename ... ARGS >
  void info(const char *what, ARGS const & ... args)
  {
    log(LVL_INFO, what, args...);
  }

  void debug(const std::string &what)
  {
    debug(what.c_str());
  }

  void debug(const char *what)
  {
    log(LVL_DEBUG, what);
  }

  void warn(const std::string &what)
  {
    warn(what.c_str());
  }

  void warn(const char *what)
  {
    log(LVL_WARN, what);
  }

  void error(const std::string &what)
  {
    error(what.c_str());
  }

  void error(const char *what)
  {
    log(LVL_ERROR, what);
  }

  void trace(const std::string &what)
  {
    trace(what.c_str());
  }

  void trace(const char *what)
  {
    log(LVL_TRACE, what);
  }

private:
  void log(level lvl, const char *what);
  template < typename ... ARGS >
  void log(level lvl, const char *what, ARGS const & ... args);

  const char* gettimestamp(char *buffer) const
  {
    time_t rawtime;
    struct tm * timeinfo;
    ::time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strftime (buffer, 80, TIMESTAMP_FORMAT, timeinfo);
    return buffer;
  }

  const char *source_ = nullptr;
  log_manager &manager_;
};

std::map<logger::level, std::string> logger::level_strings = {
  { LVL_DEBUG, "DEBUG" },
  { LVL_INFO, "INFO" },
  { LVL_WARN, "WARN" },
  { LVL_ERROR, "ERROR" },
  { LVL_TRACE, "TRACE" }
};

class log_sink
{
public:
  virtual ~log_sink() = default;

  virtual void write(const char *message, size_t size) = 0;
};

class log_manager
{
public:
  logger create_logger(const char *source)
  {
    return logger(source, *this);
  }

  void add(std::unique_ptr<log_sink> sink)
  {
    sinks.push_back(std::move(sink));
  }

  void log(const char *msg, size_t size)
  {
    for (auto &sink : sinks) {
      sink->write(msg, size);
    }
  }
private:
  std::list<std::unique_ptr<log_sink>> sinks;
};

class basic_file_sink : public log_sink
{
protected:
  basic_file_sink() = default;
  explicit basic_file_sink(FILE *f)
    : stream(f)
  {}

public:
  void write(const char *message, size_t size) override
  {
    fwrite(message, sizeof(char), size, stream);
  }

protected:
  FILE *stream = nullptr;
};

class file_sink : public basic_file_sink
{
public:
  explicit file_sink(const std::string &path)
    : file_sink(path.c_str())
  {}

  explicit file_sink(const char *path)
  {
    stream = fopen(path, "a");
    if (stream == nullptr) {
      throw std::logic_error("error opening file");
    }
  }

  ~file_sink() override
  {
    fclose(stream);
  }
};

class stdout_sink : public basic_file_sink
{
public:
  stdout_sink()
    : basic_file_sink(stdout)
  {}
};

class stderr_sink : public basic_file_sink
{
public:
  stderr_sink()
    : basic_file_sink(stderr)
  {}
};

void logger::log(level lvl, const char *what) {
  char timestamp_buffer [80];

  const char *timestamp = gettimestamp(timestamp_buffer);

  char message_buffer[1024];

  int ret = sprintf(message_buffer, "%s $$ %7s $$ %s $$ %s\n", timestamp, level_strings[lvl].c_str(), source_, what);

  manager_.log(message_buffer, ret);
}

template<typename... ARGS>
void logger::log(logger::level lvl, const char *what, ARGS const &... args)
{
  char timestamp_buffer [80];

  const char *timestamp = gettimestamp(timestamp_buffer);

  char message_buffer[912];

  sprintf(message_buffer, what, args...);

  char log_buffer[1024];

  int ret = sprintf(log_buffer, "%s $$ %7s $$ %s $$ %s\n", timestamp, level_strings[lvl].c_str(), source_, message_buffer);

  manager_.log(log_buffer, ret);
}

static log_manager logman;

}

class MyClass
{
public:
  static matador::logger LOG;

  MyClass() {
    LOG.info("created");
  }

  ~MyClass()
  {
    LOG.info("destroyed");
  }
};

matador::logger MyClass::LOG = matador::logman.create_logger("MyClass");

int main()
{
  matador::logman.add(matador::make_unique<matador::file_sink>("log.txt"));
  matador::logman.add(matador::make_unique<matador::stdout_sink>());

  matador::logger log = matador::logman.create_logger("test");

  log.info("hello info %d", 6);
  log.debug("hello debug bug bug bug");
  log.error("hello error -> fatal");
  log.warn("hello warn: attention");
  log.trace("hello trace: what happens here");

  MyClass my1;
  MyClass my2;
}
