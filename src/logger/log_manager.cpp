#include "matador/logger/log_manager.hpp"

namespace matador {

logger log_manager::create_logger(std::string source)
{
  return logger(std::move(source), default_log_domain_);
}

logger log_manager::create_logger(std::string source, const std::string &domain_name)
{
  auto log_domain = acquire_domain(domain_name);

  return logger(std::move(source), log_domain);
}

void log_manager::add_sink(sink_ptr sink)
{
  default_log_domain_->add_sink(std::move(sink));
}

void log_manager::add_sink(sink_ptr sink, const std::string &domain_name)
{
  auto log_domain = acquire_domain(domain_name);

  log_domain->add_sink(std::move(sink));
}

void log_manager::clear()
{
  log_domain_map.clear();
  default_log_domain_->clear();
}

std::shared_ptr<log_domain> log_manager::acquire_domain(const std::string &name)
{
  if (name == "default") {
    return default_log_domain_;
  }
  auto it = log_domain_map.find(name);
  if (it == log_domain_map.end()) {
    it = log_domain_map.insert(std::make_pair(name, std::make_shared<log_domain>(name))).first;
  }
  return it->second;
}

std::shared_ptr<file_sink> create_file_sink(const std::string &logfile)
{
  return std::make_shared<file_sink>(logfile);
}

std::shared_ptr<stderr_sink> create_stderr_sink()
{
  return std::make_shared<stderr_sink>();
}

std::shared_ptr<stdout_sink> create_stdout_sink()
{
  return std::make_shared<stdout_sink>();
}

std::shared_ptr<rotating_file_sink> create_rotating_file_sink(const std::string &logfile, size_t max_size, size_t file_count)
{
  return std::make_shared<rotating_file_sink>(logfile, max_size, file_count);
}

void add_log_sink(sink_ptr sink)
{
  log_manager::instance().add_sink(std::move(sink));
}

void add_log_sink(sink_ptr sink, const std::string &domain)
{
  log_manager::instance().add_sink(std::move(sink), domain);
}

logger create_logger(std::string source)
{
  return log_manager::instance().create_logger(std::move(source));
}

logger create_logger(std::string source, const std::string &domain)
{
  return log_manager::instance().create_logger(std::move(source), domain);
}
}
