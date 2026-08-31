#ifndef MATADOR_OBSERVER_LIST_COPY_CREATOR_HPP
#define MATADOR_OBSERVER_LIST_COPY_CREATOR_HPP

#include "matador/query/observer.hpp"

#include <memory>
#include <vector>

namespace matador::query::internal {
template <typename SourceType, typename DestType, template <typename> class... ObserverType>
class observer_list_copy_creator
{
public:
  using source_observer_vector = std::vector<std::unique_ptr<observer<SourceType>>>;
  using observer_vector = std::vector<std::unique_ptr<observer<DestType>>>;
  using iterator = typename observer_vector::iterator;

  static observer_vector copy_create(const source_observer_vector &source_observers) {
    observer_list_copy_creator creator(source_observers);

    return std::move(creator.observers_);
  }

private:
  explicit observer_list_copy_creator(const source_observer_vector &source_observers)
  : source_observers_(source_observers) {
    if constexpr (sizeof...(ObserverType) != 0) {
      copy_observer<ObserverType...>();
    }
  }

  template <template <typename> class FirstObserverType>
  void copy_observer() {
    try_copy_observer<FirstObserverType>();
  }

  template <template <typename> class FirstObserverType, template <typename> class NextObserverType, template <typename> class... RestObserverType>
  void copy_observer() {
    try_copy_observer<FirstObserverType>();
    copy_observer<NextObserverType, RestObserverType...>();
  }

  template <template <typename> class CurrentObserverType>
  void try_copy_observer() {
    for ( const auto &obs : source_observers_ ) {;
      if (const auto *casted_observer = dynamic_cast<const CurrentObserverType<SourceType>*>(obs.get()); casted_observer != nullptr) {
        observers_.emplace_back(std::make_unique<CurrentObserverType<DestType>>(*casted_observer));
        break;
      }
    }
  }
private:
  const source_observer_vector &source_observers_;
  observer_vector observers_;
};
}
#endif //MATADOR_OBSERVER_LIST_COPY_CREATOR_HPP