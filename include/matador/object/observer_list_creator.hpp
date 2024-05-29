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

  static void create_missing(observer_vector &observers) {
    observer_list_creator<Type, ObserverType...> creator(observers);
  }

private:
  explicit observer_list_creator(observer_vector &observers)
  : observers_(observers) {
    if constexpr (sizeof...(ObserverType) != 0) {
      build_observer<ObserverType...>();
    }
  }

  template <template <typename> class FirstObserverType>
  void build_observer() {
    try_copy_on_missing<FirstObserverType>();
  }

  template <template <typename> class FirstObserverType, template <typename> class NextObserverType, template <typename> class... RestObserverType>
  void build_observer() {
    try_copy_on_missing<FirstObserverType>();
    build_observer<NextObserverType, RestObserverType...>();
  }

  template <template <typename> class CurrentObserverType>
  void try_copy_on_missing() {
    bool is_missing{true};
    for ( const auto &obs : observers_ ) {;
      if (dynamic_cast<const CurrentObserverType<Type>*>(obs.get())) {
        is_missing = false;
        break;
      }
    }

    if (is_missing) {
      append<CurrentObserverType<Type>>();
    }
  }

  template <class CurrentObserverType>
  void append(typename std::enable_if<std::is_default_constructible<CurrentObserverType>::value || std::is_trivially_default_constructible<CurrentObserverType>::value>::type* = nullptr) {
    observers_.emplace_back(std::make_unique<CurrentObserverType>());
  }

  template <class CurrentObserverType>
  void append(typename std::enable_if<!std::is_default_constructible<CurrentObserverType>::value && !std::is_trivially_default_constructible<CurrentObserverType>::value>::type* = nullptr) {
    static_assert("no default constructor");
  }

private:
  observer_vector &observers_;
};

}
#endif //MATADOR_OBSERVER_LIST_CREATOR_HPP
