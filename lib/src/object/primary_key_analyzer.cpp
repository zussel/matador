#include "object/primary_key_analyzer.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_tree.hpp"
#include "object/serializable.hpp"
#include "object/primary_key.hpp"

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
  std::unique_ptr<serializable> obj(node_.producer->create());
  obj->serialize(*this);
}

void primary_key_analyzer::write_value(char const *, const primary_key_base &x)
{
  node_.primary_key.reset(x.clone());
}

}