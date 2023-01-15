#ifndef OOS_OBJECT_INSERTER_HPP
#define OOS_OBJECT_INSERTER_HPP

#include "matador/object/export.hpp"

#include "matador/object/prototype_node.hpp"
#include "matador/object/basic_has_many.hpp"

#include <stack>

namespace matador {
namespace detail {

/**
 * @cond MATADOR_DEV
 * @class object_inserter
 * @brief Creates objects and object_lists
 *
 * When an serializable is inserted into the serializable store
 * subsequently other serializable must be created and
 * inserted into the serializable store.
 * This class does these tasks.
 */
class MATADOR_OBJECT_API object_inserter : public object_proxy_accessor {
public:
  /**
   * @brief Creates an object_inserter instance.
   *
   * An object_inserter instance ist created for a
   * given object_store. The notify flag tells the
   * object_inserter weather the observers should be
   * notified or not.
   *
   * @param store The object_store.
   */
  explicit object_inserter(object_store &store);

  ~object_inserter() = default;

  void insert(object_proxy *proxy, bool notify);

  void reset();

  template<class T>
  void serialize(T &x);

  void on_primary_key(const char *, basic_identifier &) {}
  template<class T>
  void on_attribute(const char *, T &) { }
  void on_attribute(const char *, char *, size_t) { }
  void on_attribute(const char *, std::string &, size_t) { }

  template<class T>
  void on_belongs_to(const char *, object_ptr<T> &x, cascade_type cascade);
  template<class T>
  void on_has_one(const char *, object_ptr<T> &x, cascade_type cascade);

  template<class T, template<class ...> class C>
  void on_has_many(const char *id, basic_has_many<T, C> &x, const char*, const char*, cascade_type cascade)
  {
    on_has_many(id, x, cascade);
  }

  template<class T, template<class ...> class C>
  void on_has_many(const char *id, basic_has_many<T, C> &, cascade_type);

private:
  template < class T >
  void increment_reference_count(T) const {}
  template < class T >
  void increment_reference_count(const object_ptr<T> &pointer) const
  {
    increment_reference_count(static_cast<const object_holder&>(pointer));
  }
  void increment_reference_count(const object_holder &holder) const;
  template < class T >
  void decrement_reference_count(T) const {}
  template < class T >
  void decrement_reference_count(object_ptr<T> &pointer) const
  {
    decrement_reference_count(static_cast<object_holder&>(pointer));
  }
  void decrement_reference_count(object_holder &holder) const;

  void insert_object(object_holder &x, const std::type_index &type_index, cascade_type cascade);
  void insert_proxy(object_proxy *proxy);

  object_proxy* initialize_has_many(abstract_has_many &x);

  template < class T, class ItemHolderType >
  void insert_has_many_item(const ItemHolderType &item,
                            object_proxy *proxy,
                            const std::shared_ptr<detail::relation_endpoint<T>>& relation_info, cascade_type cascade,
                            typename std::enable_if<!matador::is_builtin<T>::value>::type* = 0);
  template < class T, class ItemHolderType >
  void insert_has_many_item(const ItemHolderType &item,
                            object_proxy *proxy,
                            const std::shared_ptr<detail::relation_endpoint<T>>& relation_info, cascade_type,
                            typename std::enable_if<matador::is_builtin<T>::value>::type* = 0);

private:
  typedef std::set<object_proxy *> t_object_proxy_set;

  t_object_proxy_set object_proxies_;

  std::stack<object_proxy *> proxy_stack_;

  object_store &ostore_;

  std::function<void(object_store&, object_proxy*)> modified_marker_;

  bool notify_ = false;
};

/// @endcond

template<class T>
void object_inserter::serialize(T &x)
{
  matador::access::serialize(*this, x);
}

template<class T>
void object_inserter::on_belongs_to(const char *, object_ptr<T> &x, cascade_type cascade)
{
  insert_object(x, std::type_index(typeid(T)), cascade);
}

template<class T>
void object_inserter::on_has_one(const char *, object_ptr<T> &x, cascade_type cascade)
{
  insert_object(x, std::type_index(typeid(T)), cascade);
}

template<class T, template<class ...> class C>
void object_inserter::on_has_many(const char *, basic_has_many<T, C> &x, cascade_type cascade)
{
  auto *proxy = initialize_has_many(x);

  if (proxy == nullptr) {
    return;
  }

  prototype_node *node = x.owner_->node();
  auto i = node->find_endpoint(std::type_index(typeid(T)));
  if (i != node->endpoint_end()) {
    x.relation_info_ = std::static_pointer_cast<relation_endpoint<T>>(i->second);
  }

  auto first = x.begin();
  auto last = x.end();

  while (first != last) {
    auto j = first++;

    insert_has_many_item(j, proxy, x.relation_info_, cascade);
  }
}

template < class T, class ItemHolderType >
void object_inserter::insert_has_many_item(const ItemHolderType &item,
                                           object_proxy *proxy,
                                           const std::shared_ptr<detail::relation_endpoint<T>>& relation_info,
                                           cascade_type cascade,
                                           typename std::enable_if<!matador::is_builtin<T>::value>::type*)
{
  if ((cascade & cascade_type::INSERT) == cascade_type::INSERT) {
    // item is not in store, insert it
    insert_proxy(this->proxy(*item));
  }
  if (!item.holder_item().is_inserted()) {
    relation_info->insert_holder(ostore_, item.holder_item(), proxy);
    relation_info->insert_value_into_foreign(item.holder_item(), proxy);
    item.holder_item().is_inserted_ = true;
  }
}

template < class T, class ItemHolderType >
void object_inserter::insert_has_many_item(const ItemHolderType &item,
                                           object_proxy *proxy,
                                           const std::shared_ptr<detail::relation_endpoint<T>>& relation_info,
                                           cascade_type,
                                           typename std::enable_if<matador::is_builtin<T>::value>::type*)
{
  if (!item.holder_item().is_inserted()) {
    relation_info->insert_holder(ostore_, item.holder_item(), proxy);
    item.holder_item().is_inserted_ = true;
  }

}

}
}

#endif //OOS_OBJECT_INSERTER_HPP
