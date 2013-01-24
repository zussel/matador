#include "database/table.hpp"
#include "database/database.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

namespace oos {

table::table(database &db, const prototype_node &node)
  : generic_object_reader<table>(this)
  , generic_object_writer<table>(this)
  , select_(db.create_statement())
  , insert_(db.create_statement())
  , update_(db.create_statement())
  , delete_(db.create_statement())
  , node_(node)
  , column_(0)
  , object_(0)
  , inserting_(false)
{
  db.prepare_statement(node, select_, insert_, update_, delete_);
}

table::~table()
{
  delete select_;
  delete insert_;
  delete update_;
  delete delete_;
}

void table::load(object_store &ostore)
{
  while (select_->step()) {
    column_ = 0;

    // create object
    object_ = node_.producer->create();
    
    object_->deserialize(*this);
  
    ostore.insert(object_);
  }
}

void table::insert(object *obj_)
{
  inserting_ = true;

  column_ = 0;
  
  obj_->serialize(*this);

  insert_->step();
  insert_->reset(true);
}

void table::update(object *obj_)
{
  inserting_ = false;

  column_ = 0;
  
  obj_->serialize(*this);

  update_->bind(++column_, (int)obj_->id());

  update_->step();
  update_->reset(true);
}

void table::remove(object *obj_)
{
  delete_->bind(1, obj_->id());
  delete_->step();
  delete_->reset(true);
}

}
