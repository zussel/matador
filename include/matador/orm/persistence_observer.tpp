#include "matador/orm/persistence_observer.hpp"
#include "matador/orm/persistence.hpp"
#include "matador/orm/table.hpp"

namespace matador {

template < class T >
void persistence_observer<T>::on_attach(prototype_node &node, T&)
{
  if (node.is_abstract()) {
    return;
  }
  if (persistence_.tables_.find(node.type()) != persistence_.tables_.end()) {
    return;
  }
  persistence_.tables_.insert(std::make_pair(node.type(), std::make_shared<table<T>>(node, persistence_)));
}

template < class T >
void persistence_observer<T>::on_detach(prototype_node &node, T&)
{
  if (node.is_abstract()) {
    return;
  }
  auto i = persistence_.tables_.find(node.type());
  if (i == persistence_.tables_.end()) {
    return;
  }
  persistence_.tables_.erase(i);
}

template<class T>
void persistence_observer<T>::on_insert(object_proxy &proxy)
{
  persistence_.register_proxy_insert(proxy);
}

template<class T>
void persistence_observer<T>::on_update(object_proxy &proxy)
{
  persistence_.register_proxy_update(proxy);
}

template<class T>
void persistence_observer<T>::on_delete(object_proxy &proxy)
{
  persistence_.register_proxy_delete(proxy);
}

}