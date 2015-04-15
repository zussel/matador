#include "database/result.hpp"
#include "database/database_exception.hpp"
#include "database/table_reader.hpp"
#include "database/table.hpp"

namespace oos {

table_reader::table_reader(table &t, object_store &ostore)
  : generic_object_reader(this)
  , ostore_(ostore)
  , table_(t)
{}


void table_reader::read(result *res)
{
  // check result
  // create object
  std::unique_ptr<object> obj(table_.node_.producer->create());

  while (res->fetch(obj.get())) {

    new_proxy_ = new object_proxy(obj.get(), nullptr);

    obj->deserialize(*this);

    obj.release();

    ostore_.insert_proxy(new_proxy_);

    obj.reset(table_.node_.producer->create());
  }
}

void table_reader::read_value(const char *, object_base_ptr &x)
{
  /*
   * check if object has primary key
   *
   */

  prototype_iterator n = ostore_.find_prototype(x.type());

  if (n != ostore_.end()) {
    prototype_node *pn = n.get();
    pn->has_primary_key;
  }

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
  database::table_map_t::iterator j = table_.db_.table_map_.find(node->type);
  prototype_node::field_prototype_map_t::const_iterator i = table_.node_.relations.find(node->type);
  if (i != table_.node_.relations.end()) {
    j->second->relation_data[i->second.second][oid].push_back(new_proxy_);
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
  if (p == ostore_.end()) {
    throw database_exception("common", "couldn't find prototype node");
  }
  if (table_.db_.is_loaded(p->type)) {
    database::relation_data_t::iterator i = table_.relation_data.find(id);
    if (i != table_.relation_data.end()) {
      database::object_map_t::iterator j = i->second.find(new_proxy_->id());
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
}


void table_reader::read_value(const char *id, primary_key_base &x)
{
  x.deserialize(id, *this);
}

}