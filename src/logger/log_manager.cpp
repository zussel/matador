#include "matador/logger/log_manager.hpp"

namespace matador {

log_level_range log_manager::default_log_level_range_ = {};

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

void log_manager::clear_all_sinks()
{
  default_log_domain_->clear();
}

void log_manager::clear_all_sinks(const std::string &domain_name)
{
  auto it = log_domain_map.find(domain_name);
  if (it != log_domain_map.end()) {
    it->second->clear();
  }
}

void log_manager::clear()
{
  log_domain_map.clear();
  default_log_domain_->clear();
}

void log_manager::max_default_log_level(log_level max_level)
{
  default_log_level_range_.max_level = max_level;
}

log_level log_manager::max_default_log_level()
{
  return default_log_level_range_.max_level;
}

void log_manager::min_default_log_level(log_level min_level)
{
  default_log_level_range_.min_level = min_level;
}

log_level log_manager::min_default_log_level()
{
  return default_log_level_range_.min_level;
}

std::shared_ptr<log_domain> log_manager::acquire_domain(const std::string &name)
{
  if (name == "default") {
    return default_log_domain_;
  }
  auto it = log_domain_map.find(name);
  if (it == log_domain_map.end()) {
    it = log_domain_map.insert(std::make_pair(name, std::make_shared<log_domain>(name, default_log_level_range_))).first;
  }
  return it->second;
}

std::shared_ptr<log_domain> log_manager::find_domain(const std::string &name)
{
  if (name == "default") {
    return default_log_domain_;
  }
  auto it = log_domain_map.find(name);
  if (it != log_domain_map.end()) {
    return it->second;
  }
  return std::shared_ptr<log_domain>();
}

void log_manager::log_default(log_level lvl, const std::string &source, const char *message)
{
  default_log_domain_->log(lvl, source, message);
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

void default_min_log_level(log_level min_lvl)
{
  log_manager::min_default_log_level(min_lvl);
}

void default_max_log_level(log_level max_lvl)
{
  log_manager::max_default_log_level(max_lvl);
}

void domain_min_log_level(const std::string &name, log_level min_lvl)
{
  auto domain = log_manager::instance().find_domain(name);
  if (domain) {
    domain->min_log_level(min_lvl);
  }
}

void domain_max_log_level(const std::string &name, log_level max_lvl)
{
  auto domain = log_manager::instance().find_domain(name);
  if (domain) {
    domain->min_log_level(max_lvl);
  }
}

void add_log_sink(sink_ptr sink)
{
  log_manager::instance().add_sink(std::move(sink));
}

void add_log_sink(sink_ptr sink, const std::string &domain)
{
  log_manager::instance().add_sink(std::move(sink), domain);
}

void clear_all_log_sinks()
{
  log_manager::instance().clear_all_sinks();
}

void clear_all_log_sinks(const std::string &domain)
{
  log_manager::instance().clear_all_sinks(domain);
}

logger create_logger(std::string source)
{
  return log_manager::instance().create_logger(std::move(source));
}

logger create_logger(std::string source, const std::string &domain)
{
  return log_manager::instance().create_logger(std::move(source), domain);
}

void log_default(log_level lvl, const std::string &source, const char *message)
{
  log_manager::instance().log_default(lvl, source, message);
}

}
