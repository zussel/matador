//
// Created by sascha on 21.03.16.
//

#ifndef OOS_SESSION_HPP
#define OOS_SESSION_HPP

#include "object/transaction.hpp"

#include "orm/persistence.hpp"

namespace oos {

class session
{
public:
  explicit session(persistence &p);

  template < class T >
  object_ptr<T> insert(T *obj)
  {
    if (store().has_transaction()) {
      return persistence_.store().insert(obj);
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      object_ptr<T> optr(persistence_.store().insert(obj));
      tr.commit();
      return optr;
    }
  }

  template < class T >
  object_ptr<T> update(const object_ptr<T> &optr)
  {
    if (store().has_transaction()) {
      store().current_transaction().on_update<T>(optr.proxy_);
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      tr.on_update<T>(optr.proxy_);
      tr.commit();
    }
    return optr;
  }

  template < class T >
  void remove(object_ptr<T> &optr)
  {
    if (store().has_transaction()) {
      store().current_transaction().on_delete<T>(optr.proxy_);
    } else {
      transaction tr(persistence_.store(), observer_);
      tr.begin();
      store().remove(optr);
//      tr.on_delete<T>(optr.proxy_);
      tr.commit();
    }
  }

  void load();

  transaction begin();

  object_store& store();
  const object_store& store() const;

private:
  void load(const persistence::table_ptr &table);

private:
  class session_observer : public transaction::observer, public action_visitor
  {
  public:
    explicit session_observer(session &s);
    virtual void on_begin();
    virtual void on_commit(transaction::t_action_vector &actions);
    virtual void on_rollback();

    virtual void visit(insert_action *act);
    virtual void visit(update_action *act);
    virtual void visit(delete_action *act);
  private:
    session &session_;
  };

private:
  persistence &persistence_;

  std::shared_ptr<transaction::observer> observer_;

};

}

#endif //OOS_SESSION_HPP
