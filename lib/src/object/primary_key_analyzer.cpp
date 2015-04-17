#include "object/primary_key_analyzer.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_tree.hpp"
#include "object/object.hpp"

namespace oos {

primary_key_analyzer::primary_key_analyzer(prototype_node &node)
  : generic_object_writer<primary_key_analyzer>(this)
  , node_(node)
{

}

primary_key_analyzer::~primary_key_analyzer() {

}

void primary_key_analyzer::analyze()
{
  std::unique_ptr<object> obj(node_.producer->create());
  obj->serialize(*this);
}

void primary_key_analyzer::write_value(char const *, const primary_key_base &x)
{
  node_.primary_key.reset(x.clone());
}

void primary_key_analyzer::write_value(const char *id, const object_base_ptr &x)
{
  return;
  const_prototype_iterator node = node_.tree->find(x.type());
  if (node == node_.tree->end()) {
    throw_object_exception("couldn't find prototype node of type '" << x.type() << "'");
  } else if (!node->has_primary_key()) {
    throw_object_exception("object of type '" << x.type() << "' has no primary key");
  }
  std::shared_ptr<primary_key_base> fk(node->primary_key->clone());
  node_.foreign_keys.insert(std::make_pair(id, fk));
}

}