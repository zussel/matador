#ifndef OOS_PERSISTENCE_OBSERVER_HPP
#define OOS_PERSISTENCE_OBSERVER_HPP

#include "oos/object/object_store_observer.hpp"
#include "oos/object/has_many_item.hpp"

namespace oos {

class persistence;

template < class T >
class persistence_observer : public object_store_observer<T>
{
public:
  persistence_observer(persistence &p) : persistence_(p) {}

  void on_attach(prototype_node &node, T &proto) override;
  void on_detach(prototype_node &node, T &proto) override;

  void on_insert(object_proxy &) override {}
  void on_update(object_proxy &) override {}
  void on_delete(object_proxy &) override {}

private:
  persistence& persistence_;
};

template < class T >
class persistence_observer<oos::has_many_item<T>> : public object_store_observer<oos::has_many_item<T>>
{
public:
  persistence_observer(persistence &p) : persistence_(p) {}

  void on_attach(prototype_node &node, has_many_item<T> &proto) override;
  void on_detach(prototype_node &node, has_many_item<T> &proto) override;

  void on_insert(object_proxy &) override {}
  void on_update(object_proxy &) override {}
  void on_delete(object_proxy &) override {}

private:
  persistence& persistence_;
};

}
#endif //OOS_PERSISTENCE_OBSERVER_HPP
