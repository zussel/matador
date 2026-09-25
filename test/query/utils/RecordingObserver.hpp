#ifndef MATADOR_RECORDING_OBSERVER_HPP
#define MATADOR_RECORDING_OBSERVER_HPP

#include "matador/utils/message_bus.hpp"

#include "matador/query/statement_cache.hpp"

#include <queue>

namespace matador::test {
class RecordingObserver final {
public:
  explicit RecordingObserver(utils::message_bus &bus);

  std::optional<utils::message> poll();

private:
  std::vector<utils::subscription> subscriptions;
  std::mutex mutex;
  std::queue<utils::message> events;
};
}

#endif //MATADOR_RECORDING_OBSERVER_HPP
