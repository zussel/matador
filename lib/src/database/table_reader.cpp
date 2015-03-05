#include <database/result.hpp>
#include "database/table_reader.hpp"
#include "database/database.hpp"

namespace oos {

table_reader::table_reader(database &db, const prototype_node &node, object_store &ostore)
  : generic_object_reader(this)
  , ostore_(ostore_)
  , db_(db)
  , node_(node)
{}


void table_reader::read(result *res)
{

}

void table_reader::read_value(const char *, object_base_ptr &x)
{
  long oid = x.id();

  if (oid == 0) {
    return;
  }

  /*
   * find object proxy with given id
   */
  object_proxy *oproxy = ostore_.find_proxy(oid);

  if (!oproxy) {
    oproxy = ostore_.create_proxy(oid);
  }

  prototype_iterator node = ostore_.find_prototype(x.type());

  /*
   * add the child object to the object proxy
   * of the parent container
   */
  database::table_map_t::iterator j = db_.table_map_.find(node->type);
  prototype_node::field_prototype_map_t::const_iterator i = node_.relations.find(node->type);
  if (i != node_.relations.end()) {
    j->second->relation_data[i->second.second][oid].push_back(oproxy);
  }

  x.reset(oproxy);
}

void table_reader::read_value(const char *id, object_container &x)
{
  /*
   * find prototype node and check if there
   * are proxies to insert for this container
   */
  prototype_iterator p = ostore_.find_prototype(x.classname());
  if (p != ostore_.end()) {
    if (db_.is_loaded(p->type)) {
      database::relation_data_t::iterator i = relation_data.find(id);
      if (i != relation_data.end()) {
        database::object_map_t::iterator j = i->second.find(object_->id());
        if (j != i->second.end()) {
          while (!j->second.empty()) {
            x.append_proxy(j->second.front());
            j->second.pop_front();
          }
        }
      }
//    } else {
//      throw database_exception("common", "couldn't object by id");
    }
  } else {
    throw database_exception("common", "couldn't find prototype node");
  }
}


void table_reader::read_value(const char *id, primary_key_base &x)
{
  x.deserialize(id, *this);
}

}