//
// Created by sascha on 12/10/15.
//

#include "oos/object/object_holder.hpp"
#include "oos/object/object_proxy.hpp"
#include "oos/object/object_store.hpp"
#include "oos/object/object_exception.hpp"

namespace oos {

object_holder::object_holder(object_holder_type holder_type)
  : type_(holder_type)
{}

object_holder::object_holder(const object_holder &x)
  : proxy_(x.proxy_)
  , oid_(x.oid_)
{
  if (proxy_) {
    oid_ = proxy_->id();
    proxy_->add(this);
  }
}

object_holder &
object_holder::operator=(const object_holder &x)
{
  if (this != &x && proxy_ != x.proxy_) {
    reset(x.proxy_, cascade_type::NONE);
  }
  return *this;
}

object_holder::object_holder(object_holder_type holder_type, object_proxy *op)
  : proxy_(op)
  , type_(holder_type)
  , oid_(0)
{
  if (proxy_) {
    oid_ = proxy_->id();
    proxy_->add(this);
  }
}

object_holder::~object_holder()
{
  if (proxy_) {
    if (is_internal() && is_inserted_) {
      --(*proxy_);
    }
    proxy_->remove(this);
    /*
     * if proxy was created temporary
     * we can delete it here
     */
    if (!proxy_->ostore() && proxy_->ptr_set_.empty()) {
      delete proxy_;
    }
  }
}

bool object_holder::operator==(const object_holder &x) const
{
  return x.proxy_ == proxy_;
}

bool object_holder::operator!=(const object_holder &x) const
{
  return !(x == *this);
}

void object_holder::reset(object_proxy *proxy, cascade_type cascade)
{
  if (proxy_ == proxy) {
    return;
  }
  if (proxy_) {
    oid_ = 0;
    if (is_internal() && is_inserted_ && proxy_->ostore_) {
      --(*proxy_);
      proxy_->ostore_->on_remove_relation_item(*proxy_->node_, proxy_, owner_);
//      proxy_->node_->notify_delete_relation(owner_, proxy);
    }
    proxy_->remove(this);
    /*
     * if proxy was created temporary
     * we can delete it here
     */
    if (!proxy_->ostore() && proxy_->ptr_set_.empty()) {
      delete proxy_;
    }
  }
  proxy_ = proxy;
  cascade_ = cascade;
  if (proxy_) {
    oid_ = proxy_->id();
    if (is_internal() && is_inserted_ && proxy_->ostore_) {
      ++(*proxy_);
      proxy_->ostore_->on_append_relation_item(*proxy_->node_, proxy_, owner_);
//      proxy_->node_->notify_insert_relation(owner_, proxy);
    }
    proxy_->add(this);
  }
}

void object_holder::clear()
{
  reset(nullptr, cascade_type::ALL);
}

bool object_holder::empty() const
{
  return proxy_ == nullptr;
}

void object_holder::reset(const std::shared_ptr<basic_identifier> &id)
{
  if (proxy_ && !proxy_->pk()->is_same_type(*id)) {
    throw object_exception("identifier types are not equal");
  }
  reset(new object_proxy(id), cascade_type::NONE);
}

bool
object_holder::is_loaded() const
{
  return (proxy_ && proxy_->obj());
}

unsigned long
object_holder::id() const
{
  return (proxy_ ? proxy_->id() : oid_);
}

void object_holder::id(unsigned long id)
{
  if (proxy_) {
    throw std::logic_error("proxy already set");
  } else {
    oid_ = id;
  }
}

object_store *object_holder::store() const
{
  return (proxy_ ? proxy_->ostore() : nullptr);
}

void*object_holder::ptr()
{
  return proxy_ ? proxy_->obj() : nullptr;
}

const void*object_holder::ptr() const
{
  return lookup_object();
}

void* object_holder::lookup_object()
{
  if (proxy_ && proxy_->obj()) {
    if (proxy_->ostore()) {
      // Todo: callback to object store
//      proxy_->ostore()->mark_modified(proxy_);
    }
    return proxy_->obj();
  } else {
    return nullptr;
  }
}

void*object_holder::lookup_object() const
{
  return proxy_ ? proxy_->obj() : nullptr;
}

bool object_holder::is_belongs_to() const
{
  return type_ == object_holder_type::BELONGS_TO;
}

bool object_holder::is_has_one() const
{
  return type_ == object_holder_type::HAS_ONE;
}

bool object_holder::is_object_ptr() const
{
  return type_ == object_holder_type::OBJECT_PTR;
}

bool object_holder::is_internal() const
{
  return type_ == object_holder_type::BELONGS_TO || type_ == object_holder_type::HAS_ONE;
}

bool object_holder::is_inserted() const
{
  return is_inserted_;
}

bool object_holder::has_primary_key() const
{
  return (proxy_ ? proxy_->has_identifier() : false);
}

std::shared_ptr<basic_identifier> object_holder::primary_key() const
{
  return (proxy_ ? proxy_->pk() : nullptr);
}

unsigned long object_holder::reference_count() const
{
  return (proxy_ ? proxy_->reference_counter_ : 0UL);
}

object_holder_type object_holder::holder_type() const
{
  return type_;
}

std::ostream& operator<<(std::ostream &out, const object_holder &x)
{
  if (x.proxy_) {
    if (x.proxy_->obj()) {
      out << "loaded object " << x.proxy_->obj();
    } else {
      out << "unload object [" << x.id() << "]";
    }
  } else {
    out << "unknown object [" << 0 << "]";
  }
  return out;
}

}