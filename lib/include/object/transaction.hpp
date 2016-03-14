//
// Created by sascha on 3/9/16.
//

#ifndef OOS_TRANSACTION_HPP
#define OOS_TRANSACTION_HPP

#include "object/action.hpp"
#include "object/object_exception.hpp"
#include "object/action_inserter.hpp"

#include "tools/byte_buffer.hpp"

#include <iostream>
#include <vector>
#include <unordered_map>

#ifdef _MSC_VER
#ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_API
#endif

namespace oos {

class object_store;

class OOS_API transaction
{
public:
  explicit transaction(object_store &store);

  void begin();
  void commit();
  void rollback();
  void abort();

  void on_insert(object_proxy *proxy);
  template < class T >
  void on_update(object_proxy *proxy);
  template < class T >
  void on_delete(object_proxy *proxy);

private:
  void backup(const action_ptr &a, const object_proxy *proxy);
  void restore(const action_ptr &a);

  void cleanup();

private:
  object_store &store_;

  typedef std::shared_ptr<action> action_ptr;
  typedef std::vector<action_ptr> t_action_vactor;
  typedef t_action_vactor::iterator action_iterator;
  typedef std::unordered_map<unsigned long, action_iterator> t_id_action_iterator_map;

  t_action_vactor actions_;
  t_id_action_iterator_map id_map_;

  action_inserter inserter_;
  byte_buffer object_buffer_;
};

template < class T >
void transaction::on_update(object_proxy *proxy)
{
  /*****************
   *
   * backup updated serializable
   * on rollback the serializable
   * is restored to old values
   *
   *****************/
  if (id_map_.find(proxy->id()) == id_map_.end()) {
    backup(std::make_shared<update_action<T>>(proxy), proxy);
  } else {
    // An serializable with that id already exists
    // do nothing because the serializable is already
    // backed up
  }
}

template < class T >
void transaction::on_delete(object_proxy *proxy)
{
  /*****************
   *
   * backup deleted serializable
   * on rollback the serializable
   * is restored into the
   * serializable store
   *
   *****************/
  t_id_action_iterator_map::iterator i = id_map_.find(proxy->id());
  if (i == id_map_.end()) {
    basic_identifier *pk = identifier_resolver::resolve(proxy->obj());
    backup(std::make_shared<delete_action>(proxy->node()->type(), proxy->id(), pk), proxy);
  } else {
    action_remover ar(action_list_);
    ar.remove(i->second, proxy);
  }
}

}
#endif //OOS_TRANSACTION_HPP
