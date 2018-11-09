#include "matador/orm/relation_resolver.hpp"
#include "relation_resolver.hpp"

namespace matador {

namespace detail {

/*******************************
 *
 * Implementation common tables
 *
 *******************************/

template < class T >
template < class V >
void relation_resolver<T, typename std::enable_if<
  !std::is_base_of<basic_has_many_to_many_item, T>::value
>::type>::serialize(const char *id, belongs_to<V> &x, cascade_type cascade)
{
  basic_identifier *pk = x.primary_key();
  if (!pk) {
    return;
  }

  auto it = belongs_to_table_map_.find(id);
  if (it == belongs_to_table_map_.end()) {
    // Todo: create orm exception
    throw_object_exception("couldn't find table with id " << id);
  }

  auto foreign_table = it->second;

  // get node of object type
  prototype_iterator node = store_->find(x.type());

  object_proxy *proxy = node->find_proxy(pk);
  if (proxy) {
    /**
     * find proxy in node map
     * if proxy can be found object was
     * already read - replace proxy
     */
    x.reset(proxy, cascade, false);
  } else {
    /**
     * if proxy can't be found we create
     * a proxy and store it in tables
     * proxy map. it will be used when
     * table is read.
     */
    auto foreign_proxy = foreign_table->find_proxy(pk);
    if (foreign_proxy == foreign_table->end_proxy()) {
      proxy = new object_proxy(pk, (T*)nullptr, node.get());
      foreign_proxy = foreign_table->insert_proxy(pk, proxy);
      // if foreign relation is HAS_ONE
      x.reset(foreign_proxy->second.proxy, cascade, false);
    } else {
      x.reset(foreign_proxy->second.proxy, cascade, true);
    }
    foreign_proxy->second.primary_keys.push_back(pk);
  }


  if (foreign_table->is_loaded()) {

    // insert into foreign endpoint
    auto belongs_to_relation = table_.node().find_endpoint(id);

    if (belongs_to_relation != table_.node().endpoint_end()) {
      belongs_to_relation->second->insert_value_into_foreign(proxy_, proxy);
    }

  } else {
    // add relation data
    auto lptr = std::static_pointer_cast<table<V>>(foreign_table);
    lptr->append_relation_data(table_.name(), proxy->pk(), object_pointer<T, object_holder_type::OBJECT_PTR>(proxy_), nullptr);
  }
}

/*******************************
 *
 * Implementation for relation
 * tables with foreign key
 *
 *******************************/

template < class T >
template < class V >
void relation_resolver<T, typename std::enable_if<
  std::is_base_of<basic_has_many_to_many_item, T>::value &&
  !matador::is_builtin<typename T::right_value_type>::value
>::type>::serialize(const char *, V &x)
{
  // must be right side value
  // if left table is loaded
  // insert it into concrete object
  // else
  // insert into relation data
  if (left_table_ptr_->is_loaded()) {
    has_many_item_holder<V> value(x, nullptr);
    left_endpoint_->insert_value_into_foreign(value, left_proxy_);
  } else {
    auto lptr = std::static_pointer_cast<table<left_value_type>>(left_table_ptr_);

    lptr->append_relation_data(table_.name(), left_proxy_->pk(), x);
  }
}

template < class T >
void relation_resolver<T, typename std::enable_if<
  std::is_base_of<basic_has_many_to_many_item, T>::value &&
  !matador::is_builtin<typename T::right_value_type>::value
>::type>::serialize(const char *, char *, size_t)
{
  // must be right side value
  // if left table is loaded
  // insert it into concrete object
  // else
  // insert into relation data
}

template < class T >
template < class V >
void relation_resolver<T, typename std::enable_if<
  std::is_base_of<basic_has_many_to_many_item, T>::value &&
  !matador::is_builtin<typename T::right_value_type>::value
>::type>::serialize(const char *, belongs_to<V> &x, cascade_type cascade)
{
  // increase reference count of has_many_to_xxx item proxy
  // because there will be an object for this kind of field
  ++(*proxy_);
  // check wether is left or right side value
  // left side will be determined first
  basic_identifier *pk = x.primary_key();
  if (!pk) {
    return;
  }

//  std::cout << "processing identifier " << *pk << " (belongs_to field: " << id << ", " << pk << ")\n";

  if (left_proxy_ == nullptr) {
    // if left is not loaded
    left_proxy_ = acquire_proxy(x, pk, cascade, left_table_ptr_);

  } else {

    object_proxy* right_proxy = acquire_proxy(x, pk, cascade, right_table_ptr_);
    if (left_table_ptr_->is_loaded()) {
      left_endpoint_->insert_value_into_foreign(right_proxy, left_proxy_);
    } else {
      auto lptr = std::static_pointer_cast<table<left_value_type>>(left_table_ptr_);
      lptr->append_relation_data(table_.name(), left_proxy_->pk(), object_pointer<right_value_type, object_holder_type::OBJECT_PTR>(right_proxy), proxy_);
    }

    if (right_table_ptr_->is_loaded()) {
      has_many_item_holder<right_value_type > holder(right_proxy, proxy_);
      right_endpoint_->insert_value_into_foreign(holder, left_proxy_);
    } else {
      auto rptr = std::static_pointer_cast<table<right_value_type>>(right_table_ptr_);
      rptr->append_relation_data(table_.name(), right_proxy->pk(), object_pointer<left_value_type, object_holder_type::OBJECT_PTR>(left_proxy_), proxy_);
    }

  }
}

/*******************************
 *
 * Implementation for relation
 * tables with builtin types
 *
 *******************************/

template < class T >
template < class V >
void relation_resolver<T, typename std::enable_if<
  std::is_base_of<basic_has_many_to_many_item, T>::value &&
  matador::is_builtin<typename T::right_value_type>::value
>::type>::serialize(const char *, V &x)
{
  // must be right side value
  // if left table is loaded
  // insert it into concrete object
  // else
  // insert into relation data
  if (left_table_ptr_->is_loaded()) {
    has_many_item_holder<V> value(x, nullptr);
    left_endpoint_->insert_value_into_foreign(value, left_proxy_);
  } else {
    auto lptr = std::static_pointer_cast<table<left_value_type>>(left_table_ptr_);

    lptr->append_relation_data(table_.name(), left_proxy_->pk(), x, proxy_);
  }
}

template < class T >
void relation_resolver<T, typename std::enable_if<
  std::is_base_of<basic_has_many_to_many_item, T>::value &&
  matador::is_builtin<typename T::right_value_type>::value
>::type>::serialize(const char *, char *, size_t)
{
  // must be right side value
  // if left table is loaded
  // insert it into concrete object
  // else
  // insert into relation data
}

template < class T >
template < class V >
void relation_resolver<T, typename std::enable_if<
  std::is_base_of<basic_has_many_to_many_item, T>::value &&
  matador::is_builtin<typename T::right_value_type>::value
>::type>::serialize(const char *, belongs_to<V> &x, cascade_type cascade)
{
  // check wether is left or right side value
  // left side will be determined first
  basic_identifier *pk = x.primary_key();
  if (!pk) {
    return;
  }

  if (left_proxy_ == nullptr) {
    // if left is not loaded
    left_proxy_ = acquire_proxy(x, pk, cascade, left_table_ptr_);

  } else {
    throw_object_exception("right value type must not be object pointer");
  }
}

}
}