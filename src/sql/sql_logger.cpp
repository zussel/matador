#include "matador/sql/sql_logger.hpp"

#include "matador/utils/os.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {

sql_logger::sql_logger()
  : sql_logger(1000000, 5)
{}

sql_logger::sql_logger(size_t max_size, size_t file_count)
  : LOG(create_logger("SQL"))
{
//  auto path = matador::os::build_path("log", "sql.log");
//  add_log_sink(create_rotating_file_sink(path, max_size, file_count));
//  add_log_sink(create_stdout_sink());
}

void sql_logger::on_execute(const std::string &stmt)
{
  LOG.info("execute: " + stmt);
}

void sql_logger::on_prepare(const std::string &stmt)
{
  LOG.info("prepare: " + stmt);
}
}