//
// Created by sascha on 3/9/16.
//

#ifndef OOS_TRANSACTION_HPP
#define OOS_TRANSACTION_HPP

#include "object/action.hpp"
#include "object/object_exception.hpp"
#include "object/action_inserter.hpp"
#include "object/action_remover.hpp"
#include "object/update_action.hpp"
#include "object/delete_action.hpp"
#include "object/object_proxy.hpp"

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
private:
  transaction(const transaction&) = delete;
  transaction& operator=(const transaction&) = delete;

public:
  typedef std::shared_ptr<action> action_ptr;
  typedef std::vector<action_ptr> t_action_vector;

public:
  struct observer
  {
    virtual ~observer() {}

    virtual void on_begin() = 0;
    virtual void on_commit(transaction::t_action_vector&) = 0;
    virtual void on_rollback() = 0;
  };

  struct null_observer : public observer
  {
    void on_begin() {};
    void on_commit(transaction::t_action_vector&) {};
    void on_rollback() {};
  };

public:
  explicit transaction(object_store &store);
  transaction(object_store &store, const std::shared_ptr<observer> &obsvr);
  transaction(const transaction &&x);
  transaction& operator=(const transaction &&x);

  void begin();
  void commit();
  void rollback();

  template < class T >
  void on_insert(object_proxy *proxy);
  template < class T >
  void on_update(object_proxy *proxy);
  template < class T >
  void on_delete(object_proxy *proxy);

private:
  typedef t_action_vector::iterator action_iterator;
  typedef std::unordered_map<unsigned long, t_action_vector::size_type> t_id_action_index_map;

  void backup(const action_ptr &a, const object_proxy *proxy);
  void restore(const action_ptr &a);

  void cleanup();

private:
  object_store &store_;

  t_action_vector actions_;
  t_id_action_index_map id_action_index_map_;

  action_inserter inserter_;
  byte_buffer object_buffer_;

  std::shared_ptr<observer> observer_;
};

template < class T >
void transaction::on_insert(object_proxy *proxy)
{
  /*****************
   *
   * backup inserted object
   * on rollback the object is removed
   * from object store
   *
   *****************/
  t_id_action_index_map::iterator i = id_action_index_map_.find(proxy->id());
  if (i == id_action_index_map_.end()) {
    // create insert action and insert serializable
    t_action_vector::size_type index = inserter_.insert<T>(proxy);
    if (index == actions_.size()) {
      throw_object_exception("transaction: action for object with id " << proxy->id() << " couldn't be inserted");
    } else {
      id_action_index_map_.insert(std::make_pair(proxy->id(), index));
    }
  } else {
    // ERROR: an serializable with that id already exists
    throw_object_exception("transaction: an object with id " << proxy->id() << " already exists");
  }
}

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
  if (id_action_index_map_.find(proxy->id()) == id_action_index_map_.end()) {
    std::shared_ptr<update_action> ua(new update_action(proxy, (T*)proxy->obj()));
    backup(ua, proxy);
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
  t_id_action_index_map::iterator i = id_action_index_map_.find(proxy->id());
  if (i == id_action_index_map_.end()) {
    backup(std::make_shared<delete_action>(proxy, (T*)proxy->obj()), proxy);
  } else {
    action_remover ar(actions_);
    ar.remove(i->second, proxy);
  }
}

}
#endif //OOS_TRANSACTION_HPP
