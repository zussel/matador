#include "matador/object/object_type_registry_entry_base.hpp"
#include "matador/object/object_store.hpp"

namespace matador {
namespace detail {

object_type_registry_entry_base::object_type_registry_entry_base(const std::type_index &ti, object_store *store,
                                                                 prototype_node *node)
  : store_(store), node_(node), type_index_(ti) {}

object_store* object_type_registry_entry_base::store() const
{
  return store_;
}

prototype_node* object_type_registry_entry_base::node() const
{
  return node_;
}

const std::type_index& object_type_registry_entry_base::type_index() const
{
  return type_index_;
}

object_inserter& object_type_registry_entry_base::inserter() const
{
  return store_->object_inserter_;
}

object_deleter& object_type_registry_entry_base::deleter() const
{
  return store_->object_deleter_;
}

void object_type_registry_entry_base::mark_modified(object_proxy *proxy)
{
  store_->mark_modified(proxy);
}

std::shared_ptr<object_type_registry_entry_base> null_object_type_registry_entry::null_type_entry { std::make_shared<null_object_type_registry_entry>() };

null_object_type_registry_entry::null_object_type_registry_entry()
: object_type_registry_entry_base(std::type_index(typeid(null_type)), nullptr, nullptr)
{}

void null_object_type_registry_entry::sync_id(object_proxy *) const {}

void null_object_type_registry_entry::restore(object_proxy *, byte_buffer &, object_deserializer &) const {}

void null_object_type_registry_entry::backup(object_proxy *, byte_buffer &, object_serializer &) const {}

void null_object_type_registry_entry::create_object(object_proxy *) const {}

void null_object_type_registry_entry::insert_object(object_proxy *) const {}

void null_object_type_registry_entry::delete_object(object_proxy *) const {}

basic_identifier* null_object_type_registry_entry::resolve_identifier(object_proxy *) const
{
  return nullptr;
}

void null_object_type_registry_entry::mark_modified(object_proxy *) {}

}
}