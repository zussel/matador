#include "database/mysql/mysql_table.hpp"
#include "database/mysql/mysql_types.hpp"
#include "database/mysql/mysql_database.hpp"

#include "database/statement_creator.hpp"

namespace oos {

namespace mysql {

class result_initializer : public object_writer
{
public:
  result_initializer(mysql_statement *stmt)
    : stmt_(stmt)
    , index_(0)
  {}
  virtual ~result_initializer() {}
  
  void prepare_result_fields(object *o)
  {
    index_ = 0;
    o->serialize(*this);
  }

protected:
	virtual void write(const char*, char)
  {
  }
	virtual void write(const char*, float)
  {
    stmt_->prepare_result_column<float>(index_++);
  }
	virtual void write(const char*, double)
  {
  }
  virtual void write(const char*, short)
  {
  }
	virtual void write(const char*, int)
  {
    stmt_->prepare_result_column<int>(index_++);
  }
	virtual void write(const char*, long)
  {
  }
  virtual void write(const char*, unsigned char)
  {
  }
  virtual void write(const char*, unsigned short)
  {
  }
  virtual void write(const char*, unsigned int)
  {
  }
  virtual void write(const char*, unsigned long)
  {
  }
	virtual void write(const char*, bool)
  {
  }
	virtual void write(const char*, const char*, int)
  {
  }
	virtual void write(const char*, const std::string&)
  {
  }
	virtual void write(const char*, const varchar_base&)
  {
  }
	virtual void write(const char*, const object_base_ptr&)
  {
  }
  virtual void write(const char*, const object_container&)
  {
  }

private:
  mysql_statement *stmt_;
  int index_;
};

mysql_table::mysql_table(mysql_database &db, const prototype_node &node)
  : table(node)
  , db_(db)
  , select_(new mysql_statement(db))
  , insert_(new mysql_statement(db))
  , update_(new mysql_statement(db))
  , delete_(new mysql_statement(db))
{
  // create dummy
  object *o = node.producer->create();
  // create string
  create_statement_creator<mysql_types> creator;
  
  std::string stmt(creator.create(o, node.type.c_str(), ""));
  std::cout << "create statement for table [" << node.type << "]: " << stmt << "\n";
  create_statement(stmt);

  // drop string
  drop_statement_creator<mysql_types> drop;
  drop_statement(drop.create(o, node.type.c_str(), ""));

  // delete dummy
  delete o;
}

void mysql_table::prepare()
{
  // create object dummy
  object *o = node().producer->create();

  // create select command
  select_statement_creator<mysql_types> select_creator;
  // state wasn't found, create sql string
  std::string sql = select_creator.create(o, node().type.c_str(), 0);
  // prepare statement
  select_->prepare(sql);
  // prepare result fields
  result_initializer ri(select_);
  ri.prepare_result_fields(o);

  // prepare insert statement
  insert_statement_creator<mysql_types> insert_creator;
  // state wasn't found, create sql string
  sql = insert_creator.create(o, node().type.c_str(), 0);
  // prepare statement
  insert_->prepare(sql);

  // prepare insert statement
  update_statement_creator<mysql_types> update_creator;
  // state wasn't found, create sql string
  sql = update_creator.create(o, node().type.c_str(), "id=?");
  // prepare statement
  update_->prepare(sql);
  
  // prepare insert statement
  delete_statement_creator<mysql_types> delete_creator;
  // state wasn't found, create sql string
  sql = delete_creator.create(o, node().type.c_str(), "id=?");
  // prepare statement
  delete_->prepare(sql);

  /*
  statement *select = query.select(o).prepare();
  statement *select_single = query.select(o).where("id=?").prepare();
  statement *insert = query.insert(o).prepare();
  statement *update = query.update(o).where("id=?").prepare();
  statement *remove = query.remove(o).where("id=?").prepare();
  */

  // delete object dummy
  delete o;
}

database& mysql_table::db()
{
  return db_;
}

const database& mysql_table::db() const
{
  return db_;
}

statement* mysql_table::select()
{
  return select_;
}

statement* mysql_table::insert()
{
  return insert_;
}

statement* mysql_table::update()
{
  return update_;
}

statement* mysql_table::remove()
{
  return delete_;
}

}

}
