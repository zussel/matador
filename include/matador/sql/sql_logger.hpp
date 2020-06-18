#ifndef MATADOR_SQL_LOGGER_HPP
#define MATADOR_SQL_LOGGER_HPP

#include "matador/sql/basic_sql_logger.hpp"
#include "matador/logger/logger.hpp"

namespace matador {

class sql_logger : public basic_sql_logger
{
public:
  sql_logger();

  void on_connect() override { }
  void on_close() override { }
  void on_execute(const std::string &stmt) override;
  void on_prepare(const std::string &stmt) override;

private:
  matador::logger LOG;
};

}
#endif //MATADOR_SQL_LOGGER_HPP
