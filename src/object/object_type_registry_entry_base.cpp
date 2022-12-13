#include "matador/object/object_type_registry_entry_base.hpp"
#include "matador/object/object_store.hpp"

namespace matador {
namespace detail {

object_type_registry_entry_base::object_type_registry_entry_base(const std::type_index &ti, object_store *store,
                                                                 prototype_node *node)
  : store_(store), node_(node), type_index_(ti) {}

object_store* object_type_registry_entry_base::store() const {
  return store_;
}

prototype_node* object_type_registry_entry_base::node() const {
  return node_;
}

const std::type_index& object_type_registry_entry_base::type_index() const {
  return type_index_;
}

object_inserter& object_type_registry_entry_base::inserter() const {
  return store_->object_inserter_;
}

object_deleter& object_type_registry_entry_base::deleter() const {
  return store_->object_deleter_;
}

}
}