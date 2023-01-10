#include "matador/object/update_action.hpp"
#include "matador/object/action_visitor.hpp"
#include "matador/object/object_proxy.hpp"

namespace matador {

update_action::update_action(object_proxy *proxy)
  : proxy_(proxy)
  , delete_action_(new delete_action(proxy))
{}

void update_action::accept(action_visitor *av)
{
  av->visit(this);
}

object_proxy* update_action::proxy()
{
  return proxy_;
}

const object_proxy* update_action::proxy() const
{
  return proxy_;
}

void update_action::backup(byte_buffer &buffer)
{
  proxy_->backup(buffer, *serializer_);
}

void update_action::restore(byte_buffer &buffer, object_store*)
{
  proxy_->restore(buffer, *deserializer_);
}

delete_action *update_action::release_delete_action()
{
  return delete_action_.release();
}

}