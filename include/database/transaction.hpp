#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include "object/object_store.hpp"
#include "database/action.hpp"

#include <memory>
#include <list>

namespace oos {

class database;

class transaction_impl : public action_visitor
{
public:
  virtual ~transaction_impl();
  
  virtual void visit(insert_action *a);
  virtual void visit(update_action *a);
  virtual void visit(delete_action *a);  
};

class transaction
{
public:
  transaction(database *db);
  ~transaction();
  
  long id() const;

  void begin();
  void commit();
  void rollback();

  database* db();
  const database* db() const;

private:
  class transaction_observer : public object_observer
  {
  public:
    transaction_observer(transaction &tr);
    virtual ~transaction_observer();

    virtual void on_insert(object *o);
    virtual void on_update(object *o);
    virtual void on_delete(object *o);
    
  private:
    transaction &tr_;
  };

private:
  friend class transaction_observer;
  
  void backup(action *a);
  void restore(action *a);

private:
  static long id_counter;

private:
  database *db_;
  long id_;
  
  transaction_impl *impl_;

  std::auto_ptr<transaction_observer> tro_;
  
  typedef std::list<action*> action_list_t;
  typedef action_list_t::iterator iterator;
  typedef action_list_t::const_iterator const_iterator;
  action_list_t action_list_;
};

}

#endif /* TRANSACTION_HPP */
