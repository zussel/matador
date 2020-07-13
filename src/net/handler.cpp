#include "matador/net/handler.hpp"

namespace matador {

time_t handler::next_timeout() const
{
  return next_timeout_;
}

time_t handler::interval() const
{
  return interval_;
}

reactor *handler::get_reactor() const
{
  return reactor_;
}

void handler::register_reactor(reactor *r)
{
  reactor_ = r;
}

void handler::schedule(time_t offset, time_t interval)
{
  next_timeout_ = ::time(nullptr) + offset;
  interval_ = interval;
}

void handler::cancel_timer()
{
  next_timeout_ = 0;
  interval_ = 0;
}

void handler::calculate_next_timeout(time_t now)
{
  if (interval_ > 0) {
    next_timeout_ = now + interval_;
  } else {
    next_timeout_ = 0;
  }
}

}
