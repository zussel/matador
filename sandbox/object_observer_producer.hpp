#ifndef MATADOR_OBJECT_OBSERVER_PRODUCER_HPP
#define MATADOR_OBJECT_OBSERVER_PRODUCER_HPP

#include "matador/object/typed_object_store_observer.hpp"

#include <memory>
#include <vector>

namespace matador {

class object_observer_producer
{
public:
  virtual ~object_observer_producer() = default;
  virtual std::unique_ptr<basic_object_store_observer> create() const = 0;
};


template<class Type>
class typed_object_observer_producer
{
public:
  virtual std::unique_ptr<basic_object_store_observer> create() const {

  }
};


class object_observer_factory
{
public:
  template<class Type>
  void register_observer(std::unique_ptr<typed_object_observer_producer<Type>> &&producer)
  {
    producers_.push_back(std::move(producer));
  }

  template<class Type>
  std::vector<typed_object_store_observer<Type>> create_observers()
  {
    std::vector<typed_object_store_observer<Type>> observers;
    for (const auto &producer : producers_) {
      observers.push_back(producer->create());
    }
    return observers;
  }
private:
  std::vector<std::unique_ptr<object_observer_producer>> producers_;
};

}
#endif //MATADOR_OBJECT_OBSERVER_PRODUCER_HPP
