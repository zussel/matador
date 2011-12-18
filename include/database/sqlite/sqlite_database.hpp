#ifndef SQLITE_DATABASE_HPP
#define SQLITE_DATABASE_HPP

#include "database/database.hpp"

#include <sqlite3.h>

namespace oos {
  
namespace sqlite {
  
class sqlite_database : public database_impl
{
public:
  sqlite_database();
  virtual ~sqlite_database();
  
private:
  sqlite3 *db_;
};

}
}

#endif /* SQLITE_DATABASE_HPP */
