#include "database/sqlite/sqlite_table.hpp"
#include "database/sqlite/sqlite_database.hpp"
#include "database/sqlite/sqlite_types.hpp"

#include "database/statement_creator.hpp"

namespace oos {

namespace sqlite {

sqlite_table::sqlite_table(sqlite_database &db, const prototype_node &node)
  : table(node)
  , db_(db)
{
  // create dummy
  object *o = node.producer->create();
  // create string
  create_statement_creator<sqlite_types> creator;
  create_statement(creator.create(o, node.type.c_str(), ""));

  // drop string
  drop_statement_creator<sqlite_types> drop;
  drop_statement(drop.create(o, node.type.c_str(), ""));

  // delete dummy
  delete o;
}

void sqlite_table::prepare()
{
}

database& sqlite_table::db()
{
  return db_;
}

const database& sqlite_table::db() const
{
  return db_;
}

}

}
