#include "object/primary_key_analyzer.hpp"

#include "object/prototype_node.hpp"
#include "object/object.hpp"

namespace oos {

primary_key_analyzer::primary_key_analyzer(prototype_node &node)
  : generic_object_reader<primary_key_analyzer>(this)
  , node_(node)
{

}

primary_key_analyzer::~primary_key_analyzer() {

}

void primary_key_analyzer::analyze(object *obj)
{
    obj->deserialize(*this);
}

void primary_key_analyzer::read_value(char const *, primary_key_base &)
{
  node_.has_primary_key = true;
}

}