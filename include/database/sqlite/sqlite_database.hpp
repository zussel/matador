#ifndef SQLITE_DATABASE_HPP
#define SQLITE_DATABASE_HPP

#ifdef WIN32
  #ifdef oos_sqlite_EXPORTS
    #define OOS_SQLITE_API __declspec(dllexport)
  #else
    #define OOS_SQLITE_API __declspec(dllimport)
  #endif
#else
  #define OOS_SQLITE_API
#endif

#include "database/database.hpp"

//#include <sqlite3.h>

struct sqlite3;

namespace oos {
  
namespace sqlite {
  
class OOS_SQLITE_API sqlite_database : public database_impl
{
public:
  sqlite_database(const std::string &db);
  virtual ~sqlite_database();
  
  virtual void visit(create_action *) {}

  /**
   * The interface for the insert action.
   */
  virtual void visit(insert_action *a);

  /**
   * The interface for the update action.
   */
  virtual void visit(update_action *a);

  /**
   * The interface for the delete action.
   */
  virtual void visit(delete_action *a);  

  /**
   * The interface for the drop table action.
   */
  virtual void visit(drop_action *) {}

  /**
   * Create the concrete transaction_impl.
   *
   * @return The concrete transaction_impl.
   */
  transaction_impl* create_transaction(transaction &tr) const;

private:
  sqlite3 *db_;
};

}
}

#endif /* SQLITE_DATABASE_HPP */
