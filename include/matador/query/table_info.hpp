#ifndef OBJECT_INFO_HPP
#define OBJECT_INFO_HPP

#include "matador/query/basic_table_info.hpp"
#include "matador/query/observer.hpp"

#include <functional>

namespace matador::query {
class schema_node;

template<typename Type>
class table_info final : public basic_table_info {
public:
  using create_func = std::function<std::unique_ptr<Type>()>;

  table_info(const schema_node& node,
              const std::shared_ptr<class table> &t,
              std::vector<std::unique_ptr<observer<Type>>>&& observers,
              create_func&& creator)
  : basic_table_info(node, t)
  , creator_(std::move(creator))
  , observers_(std::move(observers)){}

  explicit table_info(const schema_node& node)
  : basic_table_info(node, {}) {
  }

  const Type &prototype() const { return prototype_; }
  std::unique_ptr<Type> create() const { return creator_(); }

  void on_attach() const override {
    for (auto &observer : observers_) {
      observer->on_attach(node_, prototype_);
    }
  }

  void on_detach() const override {
    for (auto &observer : observers_) {
      observer->on_detach(node_, prototype_);
    }
  }

  void register_observer(std::unique_ptr<observer<Type>>&& observer) {
    observers_.push_back(std::move(observer));
  }

  const std::vector<std::unique_ptr<observer<Type>>>& observers() const {
    return observers_;
  }

private:
  Type prototype_;
  create_func creator_{[]{ return std::make_unique<Type>(); }};
  std::vector<std::unique_ptr<observer<Type>>> observers_;
};

template<typename Type>
using object_info_ref = std::reference_wrapper<const table_info<Type>>;

namespace detail {
struct null_type {};
}

using null_info = table_info<detail::null_type>;
}

#endif //OBJECT_INFO_HPP
