#include "object/object_preparator.hpp"

#include "object/object_proxy.hpp"
#include "object/object_ptr.hpp"
#include "object/object.hpp"
#include "object/object_exception.hpp"

namespace oos {


object_preparator::object_preparator(const prototype_node &node)
  : generic_object_reader<object_preparator>(this)
  , node_(node)
{}

object_preparator::~object_preparator()
{

}

void object_preparator::prepare(serializable *obj)
{
  obj->deserialize(*this);
}

void object_preparator::read_value(char const *id, object_base_ptr &x)
{
  std::unique_ptr<object_proxy> proxy(new object_proxy(nullptr));

  prototype_node::t_foreign_key_map::const_iterator i = node_.foreign_keys.find(id);
  if (i == node_.foreign_keys.end()) {
    throw_object_exception("couldn't find foreign key for serializable of type'" << x.type() << "'");
  }

  proxy->primary_key_.reset(i->second->clone());
  proxy->node_ = const_cast<prototype_node*>(&node_);

  x.reset(proxy.release());
}

}