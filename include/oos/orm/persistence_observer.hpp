#ifndef OOS_PERSISTENCE_OBSERVER_HPP
#define OOS_PERSISTENCE_OBSERVER_HPP

#include "oos/object/object_store_observer.hpp"

namespace oos {

class persistence_observer : public object_store_observer
{
public:
  void on_attach(prototype_node *node) override;

  void on_detach(prototype_node *node) override;

  void on_insert(object_proxy *proxy) override;

  void on_update(object_proxy *proxy) override;

  void on_delete(object_proxy *proxy) override;

};

}
#endif //OOS_PERSISTENCE_OBSERVER_HPP
