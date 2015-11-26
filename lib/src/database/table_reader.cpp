#include "database/result.hpp"
#include "database/database_exception.hpp"
#include "database/table_reader.hpp"
#include "database/table.hpp"

namespace oos {

table_reader::table_reader(table &t, object_store &ostore)
  : generic_deserializer(this)
  , ostore_(ostore)
  , table_(t)
{}


void table_reader::load(result<serializable> &res)
{
  auto first = res.begin();
  auto last = res.end();
  while (first != last) {
    serializable *obj = first.release();
    new_proxy_ = new object_proxy(obj);
    obj->deserialize(*this);
    ostore_.insert_proxy(new_proxy_);
    ++first;
  }
}

void table_reader::read_value(const char *, object_base_ptr &x)
{
  std::shared_ptr<basic_identifier> pk = x.primary_key();
  if (!pk) {
    return;
  }

  // get node of object type
  prototype_iterator node = ostore_.find_prototype(x.type());

  /**
   * find proxy in node map
   * if proxy can be found object was
   * already read - replace proxy
   */
  object_proxy *proxy = node->find_proxy(pk);
  if (proxy) {
    x.reset(proxy, x.is_reference());
  } else {
    // TODO: Can we call reset instead?
    x.proxy_->obj_ = node->producer->create();
    x.proxy_->node_ = node.get();
    proxy = ostore_.register_proxy(x.proxy_);
  }

  /*
   * add the child serializable to the serializable proxy
   * of the parent container
   */
  database::table_map_t::iterator j = table_.db_.table_map_.find(node->type);
  prototype_node::field_prototype_map_t::const_iterator i = table_.node_.relations.find(node->type);
  if (i != table_.node_.relations.end()) {
    j->second->relation_data[i->second.second][proxy->id()].push_back(new_proxy_);
  }
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
  if (!table_.db_.is_loaded(p->type)) {
    return;
  }
  table::t_to_many_data::iterator i = table_.relation_data.find(id);
  if (i == table_.relation_data.end()) {
    return;
  }
  /*
   * find object for container in new object
   */
  table::object_map_t::iterator j = i->second.find(new_proxy_->id());
  if (j == i->second.end()) {
    // no objects found
    return;
  }
  // move object into container
  while (!j->second.empty()) {
    x.append_proxy(j->second.front());
    j->second.pop_front();
  }
}

}
