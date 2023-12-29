#include <utility>

#include "matador/sql/basic_dialect.hpp"
#include "matador/sql/statement_impl.hpp"

namespace matador {

namespace detail {
statement_impl::statement_impl(statement_context context)
: context_(std::move(context)) {
}

const std::string& statement_impl::str() const
{
  return context_.sql;
}

const std::vector<std::string> &statement_impl::bind_vars() const
{
  return context_.bind_vars;
}

const std::vector<std::string> &statement_impl::columns() const
{
  return context_.columns;
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

bool statement_impl::is_log_enabled() const
{
  return log_enabled_;
}

const statement_context &statement_impl::context() const
{
  return context_;
}
}

}