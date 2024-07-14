#include "matador/object/prototype_node.hpp"
#include "matador/object/relation_endpoint.hpp"

namespace matador::detail {

bool basic_relation_endpoint::is_has_one() const {
  return type == HAS_ONE;
}

bool basic_relation_endpoint::is_has_many() const {
  return type == HAS_MANY;
}

bool basic_relation_endpoint::is_belongs_to() const {
  return type == BELONGS_TO;
}

void basic_relation_endpoint::insert_value_into_foreign(object_proxy *value, object_proxy *owner) const
{
  auto sptr = foreign_endpoint.lock();
  if (sptr) {
    sptr->insert_value(value, owner);
  }
}

void basic_relation_endpoint::remove_value_from_foreign(object_proxy *value, object_proxy *owner) const
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

void basic_relation_endpoint::mark_holder_as_inserted(basic_container_item_holder &holder) const
{
  holder.is_inserted_ = true;
}

void basic_relation_endpoint::mark_holder_as_removed(basic_container_item_holder &holder) const
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
  auto locked_endpoint = foreign_endpoint.lock();
  if (locked_endpoint) {
    out << " -> " << locked_endpoint->node->type() << "::" << locked_endpoint->field << " (" << locked_endpoint->type_name << ")";
  }
}

}
