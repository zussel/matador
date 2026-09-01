#ifndef COLLECTION_HPP
#define COLLECTION_HPP

#include "matador/query/collection_proxy.hpp"

namespace matador::query {

template < class Type >
class collection {
public:
  using value_type = typename collection_proxy<Type>::value_type;
  using iterator = typename collection_proxy<Type>::iterator;
  using const_iterator = typename collection_proxy<Type>::const_iterator;

  collection()
  : proxy_(std::make_shared<collection_proxy<Type>>()) {}
  explicit collection(std::shared_ptr<collection_proxy<Type>> proxy)
  : proxy_(std::move(proxy)) {}
  explicit collection(std::vector<Type> items)
  : proxy_(std::make_shared<collection_proxy<Type>>(std::move(items))) {}
  collection(const collection& other) = default;

  void push_back(const value_type& value) {
    proxy_->push_back(value);
  }

  iterator begin() {
    return proxy_->begin();
  }

  iterator end() {
    return proxy_->end();
  }

  const_iterator begin() const {
    return proxy_->begin();
  }

  const_iterator end() const {
    return proxy_->end();
  }

  [[nodiscard]] size_t size() const {
    return proxy_->size();
  }

  [[nodiscard]] bool empty() const {
    return proxy_->empty();
  }

  void reset(std::shared_ptr<abstract_collection_proxy<Type>> proxy) {
    proxy_ = std::move(proxy);
  }

  [[nodiscard]] std::shared_ptr<abstract_collection_proxy<Type>> proxy() const {
    return proxy_;
  }

private:
  std::shared_ptr<abstract_collection_proxy<Type>> proxy_;
};

}
#endif //COLLECTION_HPP
