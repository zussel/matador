#include "database/table.hpp"
#include "database/database.hpp"
#include "database/result.hpp"

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

table::table(const prototype_node &node)
  : generic_object_reader<table>(this)
  , node_(node)
  , column_(0)
  , object_(0)
  , ostore_(0)
  , inserting_(false)
  , prepared_(false)
  , is_loaded_(false)
{}

table::~table()
{
}

std::string table::name() const
{
  return node_.type;
}

void table::create()
{
  db().execute(create_.c_str());
  prepare();
}

void table::load(object_store &ostore)
{
  if (!prepared_) {
    prepare();
  }
  ostore_ = &ostore;

  result *res = select()->execute();

  while (res->fetch()) {
    column_ = 0;

    // create object
    object_ = node_.producer->create();
    
    res->get(o);
    // 
    object_->deserialize(*this);
  
    ostore.insert(object_);
  }
  ostore_ = 0;

  prototype_node::field_prototype_node_map_t::const_iterator first = node_.relations.begin();
  prototype_node::field_prototype_node_map_t::const_iterator last = node_.relations.end();
  while (first != last) {
//    std::cout << "DEBUG: checking for relation node [" << first->first << "] ...";
    database::table_map_t::iterator i = db().table_map_.find(first->first);
    if (i == db().table_map_.end()) {
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

  insert()->execute();
  insert()->fetch();
  insert()->reset(true);
}

void table::update(object *obj)
{
  inserting_ = false;

  column_ = 0;
  
  obj->serialize(*this);

  update()->bind(++column_, (int)obj->id());

  update()->execute();
  update()->fetch();
  update()->reset(true);
}

void table::remove(object *obj)
{
  remove(obj->id());
}

void table::remove(long id)
{
  remove()->bind(1, id);
  remove()->execute();
  remove()->fetch();
  remove()->reset(true);
}

void table::drop()
{
  db().execute(drop_.c_str());
}

bool table::is_loaded() const
{
  return is_loaded_;
}

void table::create_statement(const std::string &crt)
{
  create_ = crt;
}

void table::drop_statement(const std::string &drp)
{
  drop_ = drp;
}

const prototype_node& table::node() const
{
  return node_;
}

void table::read_value(const char *, object_base_ptr &x)
{
  long oid = 0;
//    std::cout << "DEBUG: reading field [" << id << "] (column: " << column_ << ")\n";
  select()->column(column_++, oid);
  
  
  if (oid == 0) {
    return;
  }
  
//    std::cout << "DEBUG: reading field [" << id << "] of type [" << x.type() << "]\n";
  object_proxy *oproxy = ostore_->find_proxy(oid);

  if (!oproxy) {
    oproxy = ostore_->create_proxy(oid);
  }

  prototype_iterator node = ostore_->find_prototype(x.type());
  
//    std::cout << "DEBUG: found prototype node [" << node->type << "]\n";

  /*
   * add the child object to the object proxy
   * of the parent container
   */
  database::table_map_t::iterator j = db().table_map_.find(node->type);
  prototype_node::field_prototype_node_map_t::const_iterator i = node_.relations.find(node->type);
  if (i != node_.relations.end()) {
//      std::cout << "DEBUG: found relation node [" << i->second.first->type << "] for field [" << i->second.second << "]\n";
    j->second->relation_data[i->second.second][oid].push_back(object_);
//      std::cout << "DEBUG: store relation data in node [" << i->second.first->type << "]->[" << i->second.second << "][" << oid << "].push_back[" << *object_ << "]\n";
  }
  
  x.reset(oproxy->obj);
}

void table::read_value(const char *id, object_container &x)
{
  /*
   * find prototype node and check if there
   * are proxies to insert for this container
   */
  prototype_iterator p = ostore_->find_prototype(x.classname());
  if (p != ostore_->end()) {
//      std::cout << "DEBUG: found container of type [" << p->type << "(" << x.classname() << ")] for prototype:field [" << node_.type << ":" << id << "]\n";
    if (db().is_loaded(p->type)) {
//        std::cout << "DEBUG: " << x.classname() << " loaded; fill in field [" << id << "] of container [" << object_->id() << "]\n";
      database::relation_data_t::iterator i = relation_data.find(id);
      if (i != relation_data.end()) {
        database::object_map_t::iterator j = i->second.find(object_->id());
        if (j != i->second.end()) {
//            std::cout << "DEBUG: found item list [" << x.classname() << "] with [" << j->second.size() << "] elements\n";
          while (!j->second.empty()) {
            x.append_proxy(j->second.front()->proxy_);
            j->second.pop_front();
          }
        }
      }
    } else {
//        std::cout << "DEBUG: " << x.classname() << " not loaded; container will be filled after of [" << x.classname() << "] load\n";
    }
  }
}

}
