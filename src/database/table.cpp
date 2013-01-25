#include "database/table.hpp"
#include "database/database.hpp"

#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

namespace oos {

class relation_filler : public generic_object_reader<relation_filler>
{
public:
  relation_filler(database::table_ptr &tbl)
    : generic_object_reader<relation_filler>(this)
    , info_(tbl)
    , object_(0)
  {}
  virtual ~relation_filler() {}
  
  void fill()
  {
    object_proxy *first = info_->node_.op_first->next;
    object_proxy *last = info_->node_.op_marker;
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
    table::relation_data_t::iterator i = info_->relation_data.find(id);
    if (i != info_->relation_data.end()) {
      table::object_map_t::iterator j = i->second.find(object_->id());
//      std::cout << "DEBUG: lookup for object [" << object_->classname() << "] id [" << object_->id() << "]\n";
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
  , prepared_(false)
  , is_loaded_(false)
{
  db_.initialize_table(node_, create_, drop_);
}

table::~table()
{
  delete select_;
  delete insert_;
  delete update_;
  delete delete_;
}

std::string table::name() const
{
  return node_.type;
}

void table::prepare()
{
  db_.prepare_table(node_, select_, insert_, update_, delete_);
  prepared_ = true;
}

void table::create()
{
  db_.execute(create_.c_str());
  prepare();
}

void table::load(object_store &ostore)
{
  if (!prepared_) {
    prepare();
  }
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
//      throw std::out_of_range("unknown key");
    } else {
      database::table_ptr tbl = i->second;
      if (tbl->is_loaded()) {
//        std::cout << " loaded\n";
        relation_filler filler(tbl);
        filler.fill();
      } else {
//        std::cout << " not loaded\n";
      }
    }

    ++first;
  }
  is_loaded_ = true;
}

void table::insert(object *obj)
{
  inserting_ = true;

  column_ = 0;
  
  obj->serialize(*this);

  insert_->step();
  insert_->reset(true);
}

void table::update(object *obj)
{
  inserting_ = false;

  column_ = 0;
  
  obj->serialize(*this);

  update_->bind(++column_, (int)obj->id());

  update_->step();
  update_->reset(true);
}

void table::remove(object *obj)
{
  remove(obj->id());
}

void table::remove(long id)
{
  delete_->bind(1, id);
  delete_->step();
  delete_->reset(true);
}

void table::drop()
{
  db_.execute(drop_.c_str());
}

bool table::is_loaded() const
{
  return is_loaded_;
}

}
