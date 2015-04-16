#include "object/object_preparator.hpp"

#include "object/object_proxy.hpp"
#include "object/object_ptr.hpp"
#include "object/prototype_tree.hpp"
#include "object/object.hpp"
#include "object/object_exception.hpp"

namespace oos {


object_preparator::object_preparator(prototype_tree &tree)
  : generic_object_reader<object_preparator>(this)
  , tree_(tree)
{}

object_preparator::~object_preparator()
{

}

void object_preparator::prepare(object *obj)
{
  obj->deserialize(*this);
}

void object_preparator::read_value(char const *, object_base_ptr &x)
{
  std::unique_ptr<object_proxy> proxy(new object_proxy(nullptr));

  const_prototype_iterator node = tree_.find(x.type());
  if (node == tree_.end()) {
    throw_object_exception("couldn't find prototype node of type '" << x.type() << "'");
  } else if (!node->has_primary_key()) {
    throw_object_exception("object of type '" << x.type() << "' has no primary key");
  }

  proxy->primary_key_.reset(node->primary_key->clone());

  x.reset(proxy.release());
}

}