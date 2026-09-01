#ifndef OBJECT_PROXY_HPP
#define OBJECT_PROXY_HPP

#include "matador/query/primary_key_resolver.hpp"

#include "matador/query/object_resolver.hpp"

#include <atomic>
#include <memory>
#include <utility>
#include <stdexcept>
#include <mutex>

namespace matador::query {
enum class object_state : uint8_t {
  Transient,
  Persistent,
  Detached,
  Removed
};

template<class Type>
class object_proxy final {
public:
  object_proxy() = default;

  // Lazy
  object_proxy(std::weak_ptr<object_resolver<Type>> resolver, const identifier& id)
  : resolver_(std::move(resolver))
  , pk_(std::move(id))
  , state_(object_state::Persistent) {
  }

  // Eager
  object_proxy(std::weak_ptr<object_resolver<Type>> resolver, std::shared_ptr<Type> obj)
  : obj_(std::move(obj))
  , resolver_(std::move(resolver))
  , pk_(obj_ ? primary_key_resolver::resolve_object(*obj_).pk : identifier{})
  , state_(obj_ ? object_state::Persistent : object_state::Detached) {
  }

  // Transient
  explicit object_proxy(std::shared_ptr<Type> obj)
  : obj_(std::move(obj))
  , pk_(obj_ ? primary_key_resolver::resolve_object(*obj_).pk : identifier{}) {
  }

  void attach(std::shared_ptr<Type> obj) {
    std::lock_guard lock(mutex_);

    obj_ = std::move(obj);
    if (!obj_) {
      pk_.clear();
      state_ = object_state::Detached;
      return;
    }

    pk_ = primary_key_resolver::resolve_object(*obj_).pk;
    state_ = object_state::Persistent;
  }

  void resolver(std::weak_ptr<object_resolver<Type>> resolver) {
    std::lock_guard lock(mutex_);
    resolver_ = std::move(resolver);
  }

  [[nodiscard]] std::shared_ptr<Type> object() const {
    return resolve_object();
  }

  void invalidate() {
    std::lock_guard lock(mutex_);
    obj_.reset();
    resolver_.reset();
    state_ = object_state::Detached;
  }

  [[nodiscard]] void *raw_pointer() const { return static_cast<void *>(pointer()); }

  Type *operator->() {
    auto *ptr = pointer();
    if (!ptr) {
      throw std::runtime_error("Cannot dereference empty object proxy");
    }
    return ptr;
  }

  const Type *operator->() const {
    auto *ptr = pointer();
    if (!ptr) {
      throw std::runtime_error("Cannot dereference empty object proxy");
    }
    return ptr;
  }

  Type &operator*() {
    auto *ptr = pointer();
    if (!ptr) {
      throw std::runtime_error("Cannot dereference empty object proxy");
    }
    return *ptr;
  }

  const Type &operator*() const {
    auto *ptr = pointer();
    if (!ptr) {
      throw std::runtime_error("Cannot dereference empty object proxy");
    }
    return *ptr;
  }

  Type *pointer() const {
    return resolve_object().get();
  }

  [[nodiscard]] bool empty() const {
    std::lock_guard lock(mutex_);
    return !obj_ && resolver_.expired();
  }

  [[nodiscard]] bool valid() const { return !empty(); }

  [[nodiscard]] bool has_primary_key() const {
    std::lock_guard lock(mutex_);
    return !pk_.is_null();
  }

  [[nodiscard]] identifier primary_key() const {
    std::lock_guard lock(mutex_);
    return pk_;
  }

  void primary_key(const identifier &pk) {
    std::lock_guard lock(mutex_);
    pk_ = pk;
  }

  bool is_persistent() const { return is_state(object_state::Persistent); }
  bool is_transient() const { return is_state(object_state::Transient); }
  bool is_detached() const { return is_state(object_state::Detached); }
  bool is_removed() const { return is_state(object_state::Removed); }

  bool is_state(const object_state state) const {
    std::lock_guard lock(mutex_);
    return state_ == state;
  }

  void change_state(const object_state state) {
    std::lock_guard lock(mutex_);
    state_ = state;
  }
private:
  std::shared_ptr<Type> resolve_object() const {
    std::shared_ptr<Type> current;
    std::shared_ptr<object_resolver<Type>> resolver;
    identifier pk;

    {
      std::lock_guard lock(mutex_);
      if (obj_) {
        return obj_;
      }

      resolver = resolver_.lock();
      if (!resolver) {
        return nullptr;
      }

      pk = pk_;
    }

    current = resolver->resolve(pk);

    {
      std::lock_guard lock(mutex_);
      if (!obj_) {
        obj_ = std::move(current);
      }
      return obj_;
    }
  }
private:
  mutable std::shared_ptr<Type> obj_{};
  mutable std::weak_ptr<object_resolver<Type>> resolver_{};
  identifier pk_{};
  object_state state_{object_state::Transient};
  mutable std::mutex mutex_{};
};
}
#endif //OBJECT_PROXY_HPP
