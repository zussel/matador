#include "object/object_proxy.hpp"
#include "object/object.hpp"

object_proxy::object_proxy(object *o, object_store *os)
  : obj(o)
  , id((o ? o->id() : 0))
  , ostore(os)
{}
object_proxy::~object_proxy()
{
  if (obj) {
    delete obj;
  }
}

void object_proxy::remove()
{
  if (prev) {
    prev->next = next;
  }
  if (next) {
    next->prev = prev;
  }
  prev = NULL;
  next = NULL;
}

void object_proxy::insert(object_proxy* oproxy)
{
  // link oproxy before this node
  oproxy->prev = prev;
  oproxy->next = this;
  if (prev) {
    prev->next = oproxy;
  }
  prev = oproxy;
}
