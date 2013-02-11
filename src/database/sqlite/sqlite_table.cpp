#include "database/sqlite/sqlite_table.hpp"
#include "database/sqlite/sqlite_database.hpp"
#include "database/sqlite/sqlite_types.hpp"

#include "database/statement_creator.hpp"

namespace oos {

namespace sqlite {

sqlite_table::sqlite_table(sqlite_database &db, const prototype_node &node)
  : table(node)
  , db_(db)
  , select_(new sqlite_statement(db))
  , insert_(new sqlite_statement(db))
  , update_(new sqlite_statement(db))
  , delete_(new sqlite_statement(db))
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
  // create dummy
  object *o = node().producer->create();

  // prepare select statement
  select_statement_creator<sqlite_types> select_creator;
  // state wasn't found, create sql string
  std::string sql = select_creator.create(o, node().type.c_str(), 0);
  // prepare statement
  select_->prepare(sql);
  
  // prepare insert statement
  insert_statement_creator<sqlite_types> insert_creator;
  // state wasn't found, create sql string
  sql = insert_creator.create(o, node().type.c_str(), 0);
  // prepare statement
  insert_->prepare(sql);
  
  // prepare insert statement
  update_statement_creator<sqlite_types> update_creator;
  // state wasn't found, create sql string
  sql = update_creator.create(o, node().type.c_str(), "id=?");
  // prepare statement
  update_->prepare(sql);
  
  // prepare insert statement
  delete_statement_creator<sqlite_types> delete_creator;
  // state wasn't found, create sql string
  sql = delete_creator.create(o, node().type.c_str(), "id=?");
  // prepare statement
  delete_->prepare(sql);
  
  // delete dummy
  delete o;
}

database& sqlite_table::db()
{
  return db_;
}

const database& sqlite_table::db() const
{
  return db_;
}

statement* sqlite_table::select()
{
  return select_;
}

statement* sqlite_table::insert()
{
  return insert_;
}

statement* sqlite_table::update()
{
  return update_;
}

statement* sqlite_table::remove()
{
  return delete_;
}

}

}
