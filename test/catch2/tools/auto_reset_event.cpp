#include "auto_reset_event.hpp"

namespace matador::test::utils {

auto_reset_event::auto_reset_event() : state(false) {}

void auto_reset_event::wait_one()
{
  std::unique_lock<std::mutex> lock(sync);
  underlying.wait(lock, [this](){return state.load();});
  state = false;
}

void auto_reset_event::set()
{
  std::unique_lock<std::mutex> lock(sync);
  state = true;
  underlying.notify_one();
}

void auto_reset_event::reset()
{
  std::unique_lock<std::mutex> lock(sync);
  state = false;
}
}