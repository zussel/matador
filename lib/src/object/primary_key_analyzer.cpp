#include "object/primary_key_analyzer.hpp"
#include "object/prototype_node.hpp"

namespace oos {

primary_key_analyzer::primary_key_analyzer(prototype_node &node)
  : node_(node)
{

}

primary_key_analyzer::~primary_key_analyzer() {

}

void primary_key_analyzer::analyze()
{
  std::unique_ptr<serializable> obj(node_.producer->create());
  obj->serialize(*this);
}

void primary_key_analyzer::serialize(char const *, const basic_identifier &x)
{
  node_.primary_key.reset(x.clone());
}

}