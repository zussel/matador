#ifndef OOS_PERSISTENCE_OBSERVER_HPP
#define OOS_PERSISTENCE_OBSERVER_HPP

#include "matador/object/object_store_observer.hpp"

namespace matador {

class persistence;

/// @cond MATADOR_DEV

class persistence_observer : public object_store_observer
{
public:

  persistence_observer(persistence &p) : persistence_(p) {}

  void on_attach(prototype_node &node) override;
  void on_detach(prototype_node &node) override;

  void on_insert(object_proxy &) override {}
  void on_update(object_proxy &) override {}
  void on_delete(object_proxy &) override {}

private:

  persistence& persistence_;
};

/// @endcond

}
#endif //OOS_PERSISTENCE_OBSERVER_HPP
