#ifndef OOS_PERSISTENCE_OBSERVER_HPP
#define OOS_PERSISTENCE_OBSERVER_HPP

#include "matador/object/typed_object_store_observer.hpp"

namespace matador {

class persistence;

/// @cond MATADOR_DEV

template < class T >
class persistence_observer : public typed_object_store_observer<T>
{
public:

  explicit persistence_observer(persistence &p) : persistence_(p) {}
  template < class O >
  explicit persistence_observer(const persistence_observer<O> *x)
    : persistence_(x->persistence_)
  {}

  void on_attach(prototype_node &node, T &proto) override;
  void on_detach(prototype_node &node, T &proto) override;

  void on_insert(object_proxy &) override {}
  void on_update(object_proxy &) override {}
  void on_delete(object_proxy &) override {}

  persistence& get_persistence() const { return persistence_; }
private:

  template < class V >
  friend class persistence_observer;

  persistence& persistence_;
};

/// @endcond

}
#endif //OOS_PERSISTENCE_OBSERVER_HPP
