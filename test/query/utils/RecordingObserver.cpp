#include "RecordingObserver.hpp"

namespace matador::test {
RecordingObserver::RecordingObserver(utils::message_bus& bus) {
  subscriptions.push_back(bus.subscribe<query::statement_accessed_event>([this](const query::statement_accessed_event &ev) {
    std::lock_guard lock(mutex);
    events.push(utils::message::from_ref(ev));
  }));
  subscriptions.push_back(bus.subscribe<query::statement_added_event>([this](const query::statement_added_event &ev) {
    std::lock_guard lock(mutex);
    events.push(utils::message::from_ref(ev));
  }));
  subscriptions.push_back(bus.subscribe<query::statement_evicted_event>([this](const query::statement_evicted_event &ev) {
    std::lock_guard lock(mutex);
    events.push(utils::message::from_ref(ev));
  }));
}

std::optional<utils::message> RecordingObserver::poll() {
  std::lock_guard lock(mutex);
  if (events.empty()) {
    return std::nullopt;
  }
  auto evt = events.front();
  events.pop();
  return evt;
}
}
