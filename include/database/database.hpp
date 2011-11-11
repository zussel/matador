#ifndef DATABASE_HPP
#define DATABASE_HPP

#ifdef WIN32
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

#include "database/action.hpp"

#include <string>
#include <stack>

namespace oos {

class object_store;
class query_result;
class transaction;
class transaction_impl;

class OOS_API database_impl : public action_visitor
{
public:
  database_impl();
  virtual ~database_impl();
  
  virtual void visit(create_action *a) {}
  virtual void visit(insert_action *a);
  virtual void visit(update_action *a);
  virtual void visit(delete_action *a);  
  virtual void visit(drop_action *a) {}
};

class OOS_API database
{
public:
  database(object_store &ostore, const std::string &dbstring);
  ~database();
  
  void open();
  void close();

  query_result* query(const std::string &q);

  object_store& ostore();
  const object_store& ostore() const;

  void push_transaction(transaction *tr);
  void pop_transaction();
  transaction* current_transaction() const;

private:
  friend class transaction;
  
  void execute_action(action *a);

private:
  database_impl *impl_;
  
  object_store &ostore_;

  std::stack<transaction*> transaction_stack_;
};

}

#endif /* DATABASE_HPP */
