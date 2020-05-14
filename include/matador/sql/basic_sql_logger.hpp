#ifndef MATADOR_BASIC_SQL_LOGGER_HPP
#define MATADOR_BASIC_SQL_LOGGER_HPP

#include <string>

namespace matador {

class basic_sql_logger
{
public:
  virtual void on_connect() = 0;
  virtual void on_close() = 0;
  virtual void on_execute(const std::string &stmt) = 0;
  virtual void on_prepare(const std::string &stmt) = 0;
};

class null_sql_logger : public basic_sql_logger
{
public:
  void on_connect() override { }
  void on_close() override { }
  void on_execute(const std::string &) override { }
  void on_prepare(const std::string &) override { }
};

}

#endif //MATADOR_BASIC_SQL_LOGGER_HPP
