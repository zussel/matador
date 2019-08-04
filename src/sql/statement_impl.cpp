//
// Created by sascha on 18.09.15.
//
#include "matador/sql/statement_impl.hpp"

namespace matador {

namespace detail {

std::string statement_impl::str() const
{
  return sql_;
}

void statement_impl::str(const std::string &s)
{
  sql_ = s;
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