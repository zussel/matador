#include "object/foreign_key_analyzer.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_tree.hpp"

namespace oos {

foreign_key_analyzer::foreign_key_analyzer(prototype_node &node)
  : node_(node)
{}

foreign_key_analyzer::~foreign_key_analyzer() {}

}
