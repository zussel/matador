#include "object/foreign_key_analyzer.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_tree.hpp"
#include "object/object.hpp"

namespace oos {

foreign_key_analyzer::foreign_key_analyzer(prototype_node &node)
  : generic_object_writer<foreign_key_analyzer>(this)
  , node_(node)
{

}

foreign_key_analyzer::~foreign_key_analyzer() {

}

void foreign_key_analyzer::analyze()
{
  std::unique_ptr<serializable> obj(node_.producer->create());
  obj->serialize(*this);
}

void foreign_key_analyzer::write_value(const char *id, const object_base_ptr &x)
{
  const_prototype_iterator node = node_.tree->find(x.type());
  if (node == node_.tree->end()) {
    throw_object_exception("couldn't find prototype node of type '" << x.type() << "'");
  } else if (!node->has_primary_key()) {
    throw_object_exception("serializable of type '" << x.type() << "' has no primary key");
  }
  std::shared_ptr<primary_key_base> fk(node->primary_key->clone());
  node_.foreign_keys.insert(std::make_pair(id, fk));
}

}