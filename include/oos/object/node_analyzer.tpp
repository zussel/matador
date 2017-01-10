#include "oos/object/node_analyzer.hpp"
#include "oos/object/prototype_iterator.hpp"
#include "oos/object/object_store.hpp"

namespace oos {
namespace detail {

template<class T, template<class ...> class ON_ATTACH>
template<class V>
void node_analyzer<T, ON_ATTACH>::serialize(V &x)
{
  oos::access::serialize(*this, x);
}

template<class T, template<class ...> class ON_ATTACH>
template<class V>
void node_analyzer<T, ON_ATTACH>::serialize(const char *id, belongs_to <V> &, cascade_type)
{
  node_.register_belongs_to(std::type_index(typeid(V)), id);
}

template<class T, template<class ...> class ON_ATTACH>
template<class V>
void node_analyzer<T, ON_ATTACH>::serialize(const char *, has_one <V> &x, cascade_type)
{
  prototype_iterator node = node_.tree()->find(x.type());
  if (node == node_.tree()->end()) {
    // if there is no such prototype node
    // prepare insertion of new node
    node = node_.tree()->template prepare_attach<V>();
  } else if (!node->has_primary_key()) {
    throw_object_exception("serializable of type '" << x.type() << "' has no primary key");
  }
}

template<class T, template<class ...> class ON_ATTACH>
template<class V, template<class ...> class C>
void node_analyzer<T, ON_ATTACH>::serialize(const char *id, has_many <V, C> &, const char *, const char *)
//void node_analyzer<T, ON_ATTACH>::serialize(const char *id, has_many<V, C> &x, const char */*owner_field*/, const char */*item_field*/)
{
  // item column column names
//  x.owner_field(owner_field);
//  x.item_field(item_field);
  // attach relation table for has many relation
  // check if has many item is already attached
  // true: check owner and item field
  // false: attach it
  prototype_iterator pi = node_.tree()->find(id);
  if (pi == node_.tree()->end()) {
    pi = node_.tree()->template attach<typename has_many<V, C>::item_type, ON_ATTACH>(id, false, nullptr, on_attach_);
  } else if (pi->type_index() == std::type_index(typeid(typename has_many<V, C>::item_type))) {
    // prototype is of type has_many_item
    throw_object_exception("many to many relations are not supported by now");
  } else {
    // throw exception
    throw_object_exception("prototype already inserted: " << pi->type());
  }
}

}
}