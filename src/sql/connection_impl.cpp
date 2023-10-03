#include "matador/sql/connection_impl.hpp"

namespace matador {
void connection_impl::enable_log()
{
  log_enabled_ = true;
}

void connection_impl::disable_log()
{
  log_enabled_ = false;
}

bool connection_impl::is_log_enabled() const
{
  return log_enabled_;
}

}