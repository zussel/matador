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

  template < class V >
  persistence_observer(const persistence_observer<V> *x)
  : persistence_ (x->persistence_)
  {}

  persistence_observer(persistence &p) : persistence_(p) {}

  void on_attach(prototype_node &node, T &proto) override;
  void on_detach(prototype_node &node, T &proto) override;

  void on_insert(object_proxy &) override {}
  void on_update(object_proxy &) override {}
  void on_delete(object_proxy &) override {}

private:
  template < class V >
  friend class persistence_observer;

  persistence& persistence_;
};

template < class T >
class persistence_observer<oos::has_many_item<T>> : public object_store_observer<oos::has_many_item<T>>
{
public:

  persistence_observer(persistence &p) : persistence_(p) {}

  typedef has_many_item<T> relation_type;

  template < class V >
  persistence_observer(const persistence_observer<V> *x);

  void on_attach(prototype_node &node, has_many_item<T> &proto) override;
  void on_detach(prototype_node &node, has_many_item<T> &proto) override;

  void on_insert(object_proxy &) override {}
  void on_update(object_proxy &) override {}
  void on_delete(object_proxy &) override {}

private:
  template < class V >
  friend class persistence_observer;

  persistence& persistence_;
  relation_type relation_;
};

}
#endif //OOS_PERSISTENCE_OBSERVER_HPP
