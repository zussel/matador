#include "matador/object/relation_field_endpoint.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/generic_access.hpp"
#include "matador/object/object_ptr.hpp"

namespace matador {
namespace detail {

template < class T, class V >
void relation_field_endpoint::set(object_store &store, object_proxy *owner, const std::string &field, object_proxy *value)
{
  if (is_insert_in_progress) {
    return;
  }
  is_insert_in_progress = true;

  store.mark_modified<T>(owner);
  matador::set(owner->obj<T>(), field, object_ptr<V>(owner));

  is_insert_in_progress = false;
}

template < class T, class V >
void relation_field_endpoint::clear(object_store &store, object_proxy *owner, const std::string &field)
{
  if (is_insert_in_progress) {
    return;
  }
  is_insert_in_progress = true;

  store.mark_modified<T>(owner);
  matador::set(owner->obj<T>(), field, object_ptr<V>());

  is_insert_in_progress = false;
}

template < class T, class V >
void relation_field_endpoint::append(object_store &store, object_proxy *owner, const std::string &field, object_proxy *value)
{
  if (is_insert_in_progress) {
    return;
  }
  is_insert_in_progress = true;

  store.mark_modified<T>(owner);
  matador::append(owner->obj<T>(), field, object_ptr<V>(owner));

  is_insert_in_progress = false;
}

template < class T, class V >
void relation_field_endpoint::remove(object_store &store, object_proxy *owner, const std::string &field, object_proxy *value)
{
  if (is_remove_in_progress) {
    return;
  }
  is_remove_in_progress = true;

  store.mark_modified<T>(owner);
  matador::remove(owner->obj<T>(), field, object_ptr<V>(owner));

  is_remove_in_progress = false;
}

}
}