#include "oos/orm/persistence_observer.hpp"
#include "oos/orm/persistence.hpp"

namespace oos {
template<class T>
void persistence_observer<T>::on_attach(prototype_node &node, T &proto)
{
  if (persistence_.tables_.find(node.type()) != persistence_.tables_.end()) {
    return;
  }
  std::cout << "node type: " << node.type() << "\n";
  persistence_.tables_.insert(std::make_pair(node.type(), std::make_shared<table<T>>(&node, persistence_)));

}

template<class T>
void persistence_observer<T>::on_detach(prototype_node &node, T &proto)
{

}

template<class T>
void persistence_observer<oos::has_many_item<T>>::on_attach(prototype_node &node, oos::has_many_item<T> &proto)
{
  if (persistence_.tables_.find(node.type()) != persistence_.tables_.end()) {
    return;
  }
  std::cout << "node type: " << node.type() << "\n";
}

template<class T>
void persistence_observer<oos::has_many_item<T>>::on_detach(prototype_node &node, oos::has_many_item<T> &proto)
{

}

}