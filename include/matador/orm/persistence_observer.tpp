#include "matador/orm/persistence_observer.hpp"
#include "matador/orm/persistence.hpp"

namespace matador {

template < class T >
void persistence_observer<T>::on_attach(prototype_node &node, T&)
{
  if (persistence_.tables_.find(node.type()) != persistence_.tables_.end()) {
    return;
  }
//  std::cout << "node type: " << node.type() << "\n";
  if (node.is_relation_node()) {
//    persistence_.tables_.insert(std::make_pair(
//      node.type(), std::make_shared<relation_table<typename relation_type::object_type>>(
//        &node, persistence_, relation_, node.node_info().owner_type_, node.node_info().relation_id_,
//        node.node_info().owner_id_column_, node.node_info().item_id_column_
//      )));
  } else {
    persistence_.tables_.insert(std::make_pair(node.type(), std::make_shared<table<T>>(&node, persistence_)));
  }

}

template < class T >
void persistence_observer<T>::on_detach(prototype_node &node, T&)
{
  auto i = persistence_.tables_.find(node.type());
  if (i == persistence_.tables_.end()) {
    return;
  }
//  std::cout << "detach node type: " << node.type() << "\n";
  persistence_.tables_.erase(i);
}

//template<class T>
//template < class V >
//persistence_observer<matador::has_many_item<T>>::persistence_observer(const persistence_observer<V> *x)
//  : persistence_ (x->persistence_)
//{
//  std::shared_ptr<basic_identifier> id(identifier_resolver<V>::resolve());
//  id->as_value(true);
//  relation_.owner(id);
//}

}