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
{
  // create dummy
  object *o = node.producer->create();
  // create string
  create_statement_creator<mysql_types> creator;
  create_statement(creator.create(o, node.type.c_str(), ""));

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
  mysql_statement *select = new mysql_statement(db_);
  select->prepare(sql);
  // prepare result fields
  result_initializer ri(select);
  ri.prepare_result_fields(o);

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

}

}
