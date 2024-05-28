#ifndef MATADOR_OBSERVER_LIST_CREATOR_HPP
#define MATADOR_OBSERVER_LIST_CREATOR_HPP

#include "matador/object/typed_object_store_observer.hpp"

#include <vector>

namespace matador {

template <typename Type, template <typename> class... ObserverType>
class observer_list_creator
{
public:
  using observer_vector = std::vector<std::unique_ptr<typed_object_store_observer<Type>>>;
  using iterator = typename observer_vector::iterator;

  observer_list_creator() {
    if constexpr (sizeof...(ObserverType) != 0) {
      build_observer<ObserverType...>();
    }
  }

  iterator begin() { return observers_.begin(); }
  iterator end() { return observers_.end(); }

private:
  template <template <typename> class FirstObserverType>
  void build_observer() {
    observers_.emplace_back(std::make_unique<FirstObserverType<Type>>());
  }

  template <template <typename> class FirstObserverType, template <typename> class NextObserverType, template <typename> class... RestObserverType>
  void build_observer() {
    observers_.emplace_back(std::make_unique<FirstObserverType<Type>>());
    build_observer<NextObserverType, RestObserverType...>();
  }

private:
  observer_vector observers_;
};

}
#endif //MATADOR_OBSERVER_LIST_CREATOR_HPP
