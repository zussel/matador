#include "matador/utils/message_bus.hpp"

namespace matador::utils {
message::message()
: ptr_(nullptr)
, owner_(nullptr)
, type_(typeid(void)) {}

bool message::empty() const {
  return ptr_ == nullptr && !owner_;
}

std::type_index message::type() const {
  return type_;
}

const void * message::raw_ptr() const {
  return owner_ ? owner_.get() : ptr_;
}

subscription::subscription(message_bus *bus, const std::type_index type, const uint64_t id)
: bus_(bus)
, type_(type)
, id_(id) {}

subscription::subscription(subscription &&other) noexcept
: bus_(other.bus_)
, type_(other.type_)
, id_(other.id_) {
  other.bus_ = nullptr;
}

subscription& subscription::operator=(subscription &&other) noexcept {
  if (this != &other) {
    unsubscribe();
    bus_ = other.bus_;
    type_ = other.type_;
    id_ = other.id_;
    other.bus_ = nullptr;
  }
  return *this;
}

subscription::~subscription() {
  unsubscribe();
}

bool subscription::valid() const {
  return bus_ != nullptr;
}

message_bus::message_bus()
: next_id_{1} {}

void message_bus::unsubscribe(const std::type_index type, HandlerId id) {
  std::unique_lock writeLock(mutex_);
  const auto it = handlers_.find(type);
  if (it == handlers_.end()) {
    return;
  }
  auto &handler_vector = it->second;
  handler_vector.erase(std::remove_if(handler_vector.begin(), handler_vector.end(),
                           [id](const Entry &e) { return e.id == id; }),
            handler_vector.end());
  if (handler_vector.empty()) {
    handlers_.erase(it);
  }
}

void message_bus::unsubscribe(HandlerId id) {
  std::unique_lock writeLock(mutex_);
  for (auto it = handlers_.begin(); it != handlers_.end(); ) {
    auto &handler_vector = it->second;
    handler_vector.erase(std::remove_if(handler_vector.begin(), handler_vector.end(),
                             [id](const Entry &e) { return e.id == id; }),
              handler_vector.end());
    if (handler_vector.empty()) it = handlers_.erase(it);
    else ++it;
  }
}

void message_bus::publish(const message &msg) const {
  if (msg.empty()) {
    return;
  }
  std::vector<Entry> snapshot;
  {
    std::shared_lock readLock(mutex_);
    const auto it = handlers_.find(msg.type());
    if (it == handlers_.end()) {
      return;
    }
    snapshot = it->second;
  }
  const void* p = msg.raw_ptr();
  for (const auto &e : snapshot) {
    if (!e.filter || e.filter(p)) e.handler(p);
  }
}
}
