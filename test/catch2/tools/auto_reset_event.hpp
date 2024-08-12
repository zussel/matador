#ifndef QUERY_AUTO_RESET_EVENT_HPP
#define QUERY_AUTO_RESET_EVENT_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace matador::test::utils {

class auto_reset_event
{
public:
  auto_reset_event();
  auto_reset_event(const auto_reset_event& other) = delete;

  void wait_one();
  void set();
  void reset();

private:
  std::condition_variable underlying;
  std::mutex sync;
  std::atomic<bool> state;
};


}

#endif //QUERY_AUTO_RESET_EVENT_HPP
