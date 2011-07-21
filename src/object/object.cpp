#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/object_atomizer.hpp"

object::object()
	: id_(0)
  , proxy_(0)
{
}

object::~object()
{
}

void object::read_from(object_atomizer *a)
{
	a->read_long("id", id_);
}

void object::write_to(object_atomizer *a) const
{
	a->write_long("id", id_);
}

const char* object::type() const
{
	return NULL;
}

long object::id() const
{
	return id_;
}

void object::id(long oid)
{
	id_ = oid;
}

object_store* object::ostore() const
{
  return proxy_ ? proxy_->ostore : 0;
}

/*
bool object::delete_object()
{
  return proxy_->ostore->remove(this);
}
*/

void object::mark_modified()
{
  if (!proxy_ || !proxy_->ostore) {
    // throw exception
    return;
  }
  proxy_->ostore->mark_modified(proxy_);
}

std::ostream& operator <<(std::ostream &os, const object &o)
{
  os << "object " << typeid(o).name() << " (" << &o << ") [" << o.id_ << "]";
  return os;
}
