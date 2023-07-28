#include "matador/sql/statement_impl.hpp"

namespace matador {

namespace detail {

statement_impl::statement_impl(basic_dialect *dialect, const matador::sql &stmt)
{
  std::tie(sql_, bind_vars_, columns_) = dialect->prepare(stmt);

}

std::string statement_impl::str() const
{
  return sql_;
}

const std::vector<std::string> &statement_impl::bind_vars() const
{
  return bind_vars_;
}

const std::vector<std::string> &statement_impl::columns() const
{
  return columns_;
}

bool statement_impl::is_valid_host_var(const std::string &host_var, size_t pos) const
{
  const auto host_var_at_pos = bind_vars().at(pos);

  return host_var_at_pos == host_var;
}

void statement_impl::log(const std::string &stmt) const
{
  if (log_enabled_) {
    std::cout << "SQL: " << stmt << "\n";
  }
}

void statement_impl::enable_log()
{
  log_enabled_ = true;
}

void statement_impl::disable_log()
{
  log_enabled_ = false;
}

bool statement_impl::is_log_enabled() const {
  return log_enabled_;
}
}

}