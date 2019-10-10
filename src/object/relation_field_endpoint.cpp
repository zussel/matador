//
// @author sascha on 3/23/17.
//

#include "matador/object/prototype_node.hpp"
#include "matador/object/relation_field_endpoint.hpp"

#include <matador/object/object_proxy.hpp>

namespace matador {

namespace detail {

void basic_relation_endpoint::set_has_many_item_proxy(basic_has_many_item_holder &holder, const object_holder &obj)
{
  set_has_many_item_proxy(holder, obj.proxy_);
}

void basic_relation_endpoint::set_has_many_item_proxy(basic_has_many_item_holder &holder, object_proxy *proxy)
{
  holder.has_many_to_many_item_poxy_ = proxy;
}


void basic_relation_endpoint::insert_value_into_foreign(object_proxy *value, object_proxy *owner)
{
  auto sptr = foreign_endpoint.lock();
  if (sptr) {
    sptr->insert_value(value, owner);
  }
}

void basic_relation_endpoint::remove_value_from_foreign(object_proxy *value, object_proxy *owner)
{
  auto sptr = foreign_endpoint.lock();
  if (sptr) {
    sptr->remove_value(value, owner);
  }
}

void basic_relation_endpoint::increment_reference_count(const object_holder &holder)
{
  ++(*(this->proxy(holder)));
}

void basic_relation_endpoint::decrement_reference_count(const object_holder &holder)
{
  --(*(this->proxy(holder)));
}

void basic_relation_endpoint::mark_holder_as_inserted(basic_has_many_item_holder &holder) const
{
  holder.is_inserted_ = true;
}

void basic_relation_endpoint::mark_holder_as_removed(basic_has_many_item_holder &holder) const
{
  holder.is_inserted_ = false;
}

std::ostream& operator<<(std::ostream &stream, const basic_relation_endpoint &endpoint)
{
  endpoint.print(stream);
  return stream;
}

void basic_relation_endpoint::print(std::ostream &out) const
{
  out << "relation " << node->type() << "::" << field << " (" << type_name << ")";
}

relation_endpoint::relation_endpoint(const std::string &field, prototype_node *node,
                                     basic_relation_endpoint::relation_type type)
: basic_relation_endpoint(field, node, type)
{}
}
}