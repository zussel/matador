#ifndef OBJECT_PTR_HPP
#define OBJECT_PTR_HPP

#include "matador/query/identifier.hpp"
#include "matador/query/object_proxy.hpp"

#include <memory>

namespace matador::query {
struct null_object_ptr_t {
  constexpr null_object_ptr_t() = default;
};

inline constexpr null_object_ptr_t nullobj{};

template <typename Type>
class object_ptr final {
public:
  object_ptr()
  : proxy_(std::make_shared<object_proxy<Type>>()) {}

  object_ptr(null_object_ptr_t) {}

  explicit object_ptr(std::shared_ptr<Type> obj)
  : proxy_(std::make_shared<object_proxy<Type>>(std::move(obj))) {}

  explicit object_ptr(std::shared_ptr<object_proxy<Type>> obj)
  : proxy_(std::move(obj)) {}

  object_ptr(const object_ptr &other) = default;
  object_ptr(object_ptr &&other) noexcept = default;
  object_ptr& operator=(const object_ptr &other) = default;
  object_ptr& operator=(object_ptr &&other) noexcept = default;

  object_ptr& operator=(null_object_ptr_t) {
    proxy_.reset();
    return *this;
  }

  bool operator==(const object_ptr &other) const {
    if (proxy_ == other.proxy_) {
      return true;
    }

    if (!proxy_ || !other.proxy_) {
      return false;
    }

    if (has_primary_key() && other.has_primary_key()) {
      return primary_key() == other.primary_key();
    }

    return false;
  }

  bool operator==(null_object_ptr_t) const {
    return empty();
  }

  bool operator!=(const object_ptr &other) const { return !operator==(other); }
  bool operator!=(null_object_ptr_t) const { return !empty(); }

  using value_type = Type;

  Type *operator->() const {
    return checked_get();
  }

  Type &operator*() {
    return *checked_get();
  }

  const Type &operator*() const {
    return *checked_get();
  }

  [[nodiscard]] bool empty() const {
    return proxy_ == nullptr || proxy_->empty();
  }

  Type *get() const {
    return proxy_ ? proxy_->pointer() : nullptr;
  }

  [[nodiscard]] std::shared_ptr<Type> object() const {
    return proxy_ ? proxy_->object() : nullptr;
  }

  void reset() {
    proxy_.reset();
  }

  void reset(std::shared_ptr<object_proxy<Type>> proxy) {
    proxy_ = std::move(proxy);
  }

  [[nodiscard]] std::shared_ptr<object_proxy<Type>> proxy() const {
    return proxy_;
  }

  explicit operator bool() const {
    return valid();
  }

  [[nodiscard]] bool valid() const {
    return proxy_ != nullptr && !proxy_->empty();
  }

  [[nodiscard]] bool has_primary_key() const {
    return proxy_ != nullptr && proxy_->has_primary_key();
  }

  [[nodiscard]] identifier primary_key() const {
    return proxy_ ? proxy_->primary_key() : identifier{};
  }

  void primary_key(const identifier &pk) {
    ensure_proxy();
    proxy_->primary_key(pk);
  }

  [[nodiscard]] bool is_persistent() const {
    return proxy_ != nullptr && proxy_->is_persistent();
  }

  [[nodiscard]] bool is_transient() const {
    return proxy_ != nullptr && proxy_->is_transient();
  }

  [[nodiscard]] bool is_detached() const {
    return proxy_ != nullptr && proxy_->is_detached();
  }

  [[nodiscard]] bool is_removed() const {
    return proxy_ != nullptr && proxy_->is_removed();
  }

  [[nodiscard]] bool is_state(const object_state state) const {
    return proxy_ != nullptr && proxy_->is_state(state);
  }

  void change_state(object_state s) const {
    if (proxy_) {
      proxy_->change_state(s);
    }
  }

private:
  Type *checked_get() const {
    auto *ptr = get();
    if (!ptr) {
      throw std::runtime_error("Cannot dereference empty object_ptr");
    }
    return ptr;
  }

  void ensure_proxy() {
    if (!proxy_) {
      proxy_ = std::make_shared<object_proxy<Type>>();
    }
  }

private:
  std::shared_ptr<object_proxy<Type>> proxy_{};
};

template<typename>
struct is_object_ptr : std::false_type {
};

template<typename Type>
struct is_object_ptr<object_ptr<Type> > : std::true_type {
};

template<class Type, typename... Args>
object_ptr<Type> make_object(Args &&... args) {
  return object_ptr<Type>(std::make_shared<Type>(std::forward<Args>(args)...));
}
}

namespace matador {
template <typename Type>
using object_ptr = query::object_ptr<Type>;
}

#endif //OBJECT_PTR_HPP
