#include "database/table.hpp"
#include "database/database.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

namespace oos {

class relation_filler2 : public generic_object_reader<relation_filler2>
{
public:
  relation_filler2(database::table_ptr &tbl)
    : generic_object_reader<relation_filler2>(this)
    , info_(tbl)
    , object_(0)
  {}
  virtual ~relation_filler2() {}
  
  void fill()
  {
    object_proxy *first = info_.node->op_first->next;
    object_proxy *last = info_.node->op_marker;
    while (first != last) {
      object_ = first->obj;
      object_->deserialize(*this);
      first = first->next;
    }
  }

  template < class T >
  void read_value(const char*, T&) {}
  
  void read_value(const char*, char*, int) {}
  
  void read_value(const char *id, object_container &x)
  {
//    std::cout << "DEBUG: fill container [" << id << "]\n";
    database::relation_data_t::iterator i = info_->relation_data.find(id);
    if (i != info_->relation_data.end()) {
      database::object_map_t::iterator j = i->second.find(object_->id());
      if (j != i->second.end()) {
//        std::cout << "DEBUG: found item list [" << x.classname() << "] with [" << j->second.size() << "] elements\n";
        while (!j->second.empty()) {
          x.append_proxy(j->second.front()->proxy_);
          j->second.pop_front();
        }
      }
    }
  }

private:
  database::table_ptr &info_;
  object *object_;
};

table::table(database &db, const prototype_node &node)
  : generic_object_reader<table>(this)
  , generic_object_writer<table>(this)
  , select_(db.create_statement())
  , insert_(db.create_statement())
  , update_(db.create_statement())
  , delete_(db.create_statement())
  , db_(db)
  , node_(node)
  , column_(0)
  , object_(0)
  , ostore_(0)
  , inserting_(false)
  , is_loaded_(false)
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
  ostore_ = &ostore;
  while (select_->step()) {
    column_ = 0;

    // create object
    object_ = node_.producer->create();
    
    object_->deserialize(*this);
  
    ostore.insert(object_);
  }
  ostore_ = 0;

  prototype_node::field_prototype_node_map_t::const_iterator first = node_.relations.begin();
  prototype_node::field_prototype_node_map_t::const_iterator last = node_.relations.end();
  while (first != last) {
//    std::cout << "DEBUG: checking for relation node [" << first->first << "] ...";
    database::table_map_t::iterator i = db_.table_map_.find(first->first);
    if (i == db_.table_map_.end()) {
      throw std::out_of_range("unknown key");
    } else {
      database::table_ptr tbl = i->second;
      if (tbl->is_loaded()) {
  //      std::cout << " loaded\n";
        relation_filler2 filler(tbl);
        filler.fill();
      } else {
  //      std::cout << " not loaded\n";
      }
    }

    ++first;
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

bool table::is_loaded() const
{
  return is_loaded_;
}

}
