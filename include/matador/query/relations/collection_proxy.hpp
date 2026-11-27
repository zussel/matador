#ifndef MATADOR_COLLECTION_PROXY_HPP
#define MATADOR_COLLECTION_PROXY_HPP

#include "matador/query/relations/collection_resolver.hpp"
#include "matador/query/relations/object_ptr.hpp"

#include "matador/query/model/identifier.hpp"

#include <algorithm>
#include <atomic>
#include <mutex>
#include <vector>

namespace matador::query {
enum class collection_proxy_state {
  transient,  // not bound to session, standalone collection
  attached,   // bound to session with resolver
  detached    // was attached, session/resolver expired
};

// has many primitive
// relation<OwnerType, PrimitiveType>
// has many ptr
// relation<OwnerType, ObjectType>
// has many to many
// relation<OwnerType, ForeignType>

template<typename Type>
class collection_proxy_iterator {
public:
  using value_type = Type;
  using reference = Type&;
  using pointer = Type*;
  using iterator_category = std::forward_iterator_tag;

  collection_proxy_iterator() = default;
  collection_proxy_iterator(typename std::vector<value_type>::iterator it)
  : it_(std::move(it)) {}

  reference operator*() {
    return *it_;
  }

  pointer operator->() {
    return &(*it_);
  }

  collection_proxy_iterator& operator++() {
    ++it_;
    return *this;
  }
  bool operator==(const collection_proxy_iterator& other) const {
    return it_ == other.it_;
  }
  bool operator!=(const collection_proxy_iterator& other) const {
    return !operator==(other);
  }

private:
  typename std::vector<value_type>::iterator it_;
};

template<typename Type>
class collection_proxy_const_iterator {
public:
  using value_type = Type;
  using reference = const Type&;
  using pointer = const Type*;
  using iterator_category = std::forward_iterator_tag;

  collection_proxy_const_iterator() = default;
  collection_proxy_const_iterator(typename std::vector<value_type>::const_iterator it)
  : it_(std::move(it)) {}

  // Allow construction from non-const iterator
  collection_proxy_const_iterator(const collection_proxy_iterator<Type>& other)
  : it_(other.it_) {}

  reference operator*() const {
    return *it_;
  }

  pointer operator->() const {
    return &(*it_);
  }

  collection_proxy_const_iterator& operator++() {
    ++it_;
    return *this;
  }

  bool operator==(const collection_proxy_const_iterator& other) const {
    return it_ == other.it_;
  }

  bool operator!=(const collection_proxy_const_iterator& other) const {
    return !operator==(other);
  }

private:
  typename std::vector<value_type>::const_iterator it_;
  // Allow const_iterator to access non-const iterator's data
  template<typename>
  friend class collection_proxy_const_iterator;
};

template<typename Type>
class abstract_collection_proxy {
public:
  using value_type = Type;
  using iterator = collection_proxy_iterator<value_type>;
  using const_iterator = collection_proxy_const_iterator<value_type>;

  virtual ~abstract_collection_proxy() = default;

  virtual void attach(std::weak_ptr<collection_resolver<Type>> resolver, identifier owner_id) = 0;

  virtual void push_back(const value_type& value) = 0;

  virtual iterator begin() = 0;
  virtual iterator end() = 0;

  virtual const_iterator begin() const = 0;
  virtual const_iterator end() const = 0;
  virtual const_iterator cbegin() const = 0;
  virtual const_iterator cend() const = 0;

  [[nodiscard]] virtual size_t size() = 0;
  [[nodiscard]] virtual size_t size() const = 0;
  [[nodiscard]] virtual bool empty() = 0;
  [[nodiscard]] virtual bool empty() const = 0;

  [[nodiscard]] virtual const identifier& owner_id() const = 0;

  void owner_info(const std::pair<std::type_index, identifier>& info) {
    owner_info_ = info;
  }

protected:
  abstract_collection_proxy() = default;
  explicit abstract_collection_proxy(const std::vector<value_type>& items)
  : items_(items) {}

protected:
  std::vector<value_type> items_;
  std::pair<std::type_index, identifier> owner_info_{typeid(void), {}};
};

template<typename Type>
class collection_proxy : public abstract_collection_proxy<Type> {
public:
  using value_type = Type;
  using iterator = typename abstract_collection_proxy<Type>::iterator;
  using const_iterator = typename abstract_collection_proxy<Type>::const_iterator;

  collection_proxy() = default;

  // Lazy - creates attached proxy that resolves on first access
  collection_proxy(std::weak_ptr<collection_resolver<Type>> resolver, identifier owner_id)
  : owner_id_(std::move(owner_id)), resolver_(std::move(resolver)), state_(collection_proxy_state::attached) {}

  // Eager - creates attached proxy with pre-loaded items
  collection_proxy(std::weak_ptr<collection_resolver<Type>> resolver, std::vector<Type> items)
  : abstract_collection_proxy<Type>(items)
  , loaded_{true}, resolver_(std::move(resolver)), state_(collection_proxy_state::attached) {}

  // Transient - creates standalone collection not bound to session
  explicit collection_proxy(std::vector<Type> items)
  : abstract_collection_proxy<Type>(items)
  , loaded_{true} {}

  [[nodiscard]] const identifier& owner_id() const override {
    return owner_id_;
  }

  [[nodiscard]] collection_proxy_state state() const {
    return state_;
  }

  void attach(std::weak_ptr<collection_resolver<Type>> resolver, identifier owner_id) override {
    std::lock_guard lock(mutex_);
    if (state_ != collection_proxy_state::transient) {
      return;
    }
    if (resolver.expired() || !owner_id.is_valid()) {
      throw std::invalid_argument("Collection proxy requires a resolver and a valid owner id");
    }
    owner_id_ = std::move(owner_id);
    resolver_ = std::move(resolver);
    state_ = collection_proxy_state::attached;
    loaded_ = false;
  }

  iterator begin() override {
    resolve();
    return this->items_.begin();
  }

  iterator end() override {
    resolve();
    return this->items_.end();
  }

  const_iterator begin() const override {
    const_cast<collection_proxy*>(this)->resolve();
    return this->items_.cbegin();
  }

  const_iterator end() const override {
    const_cast<collection_proxy*>(this)->resolve();
    return this->items_.cend();
  }

  const_iterator cbegin() const override {
    const_cast<collection_proxy*>(this)->resolve();
    return this->items_.cbegin();
  }

  const_iterator cend() const override {
    const_cast<collection_proxy*>(this)->resolve();
    return this->items_.cend();
  }

  void push_back(const value_type& value) override {
    resolve();
    this->items_.emplace_back(value);
  }

  [[nodiscard]] size_t size() override {
    resolve();
    return this->items_.size();
  }

  [[nodiscard]] size_t size() const override {
    const_cast<collection_proxy*>(this)->resolve();
    return this->items_.size();
  }

  [[nodiscard]] bool empty() override {
    resolve();
    return this->items_.empty();
  }

  [[nodiscard]] bool empty() const override {
    const_cast<collection_proxy*>(this)->resolve();
    return this->items_.empty();
  }

private:
  void resolve() {
    // Double-checked locking pattern: check before acquiring lock
    if (loaded_) {
      return;
    }

    std::lock_guard lock(mutex_);

    // Check again inside the lock to prevent TOCTOU race
    if (loaded_) {
      return;
    }

    auto resolver = resolver_.lock();
    if (!resolver) {
      if (state_ != collection_proxy_state::transient) {
        state_ = collection_proxy_state::detached;
        throw std::runtime_error("Detached collection proxy: resolver expired (session may have ended)");
      }
      return;
    }

    auto resolved = resolver->resolve(owner_id_);
    auto merged = this->items_;
    for (const auto &item : resolved) {
      if constexpr (is_object_ptr<Type>::value) {
        const auto key = item.primary_key();
        if (item.is_persistent() && key.is_valid() &&
            std::any_of(merged.begin(), merged.end(), [&key](const auto &existing) {
              return existing.is_persistent() && existing.primary_key() == key;
            })) {
          continue;
        }
      }
      merged.push_back(item);
    }
    this->items_ = std::move(merged);
    loaded_ = true;
  }

private:
  identifier owner_id_;
  std::atomic_bool loaded_{false};
  std::weak_ptr<collection_resolver<Type>> resolver_{};
  mutable std::mutex mutex_{};
  collection_proxy_state state_{collection_proxy_state::transient};
};
}
#endif //MATADOR_COLLECTION_PROXY_HPP