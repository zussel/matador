//
// Created by sascha on 12/10/15.
//

#include "matador/object/object_holder.hpp"
#include "matador/object/object_proxy.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/object_exception.hpp"

namespace matador {

object_holder::object_holder(object_holder_type holder_type)
  : type_(holder_type)
{}

object_holder::object_holder(const object_holder &x)
  : oid_(x.oid_)
{
  reset(x.proxy_, x.cascade_);
}

object_holder::object_holder(object_holder &&x) noexcept
{
  object_proxy *proxy = x.proxy_;
  x.reset(nullptr, x.cascade_, false);
  reset(proxy, x.cascade_, true);
  cascade_ = x.cascade_;
  x.owner_ = nullptr;
}

object_holder &
object_holder::operator=(const object_holder &x)
{
  if (this != &x && proxy_ != x.proxy_) {
    relation_info_ = x.relation_info_;
    reset(x.proxy_, cascade_type::NONE);
  }
  return *this;
}

object_holder &object_holder::operator=(object_holder &&x) noexcept
{
  if (this != &x) {
    object_proxy *proxy = x.proxy_;
    x.reset(nullptr, x.cascade_, false);
    reset(proxy, x.cascade_, true);
    cascade_ = x.cascade_;
    x.owner_ = nullptr;
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

bool object_holder::operator==(std::nullptr_t) const
{
  return proxy_ == nullptr;
}

bool object_holder::operator!=(const object_holder &x) const
{
  return !(x == *this);
}

bool object_holder::operator!=(std::nullptr_t) const
{
  return !(*this == nullptr);
}

void object_holder::reset(object_proxy *proxy, cascade_type cascade)
{
  reset(proxy, cascade, true);
}

void object_holder::reset(object_holder &holder)
{
  reset(holder.proxy_, holder.cascade_);
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
      out << "loaded object " << x.proxy_->obj() << " (proxy: " << x.proxy_ << ")";
    } else {
      out << "unloaded object [" << x.id() << "]";
    }
  } else {
    out << "unknown object [null]";
  }
  return out;
}

void object_holder::reset(object_proxy *proxy, cascade_type cascade, bool notify_foreign_relation)
{
  if (proxy_ == proxy) {
    return;
  }
  if (proxy_) {
    oid_ = 0;
    proxy_->remove(this);
    if (is_internal() && is_inserted_ && proxy_->ostore_) {
      --(*proxy_);

      if (relation_info_ && notify_foreign_relation) {
        relation_info_->remove_value_from_foreign(owner_, proxy_);
      }
    }
    /*
     * if proxy was created temporary
     * we can delete it here
     */
    if (proxy_ && !proxy_->ostore() && proxy_->ptr_set_.empty()) {
      delete proxy_;
    }
  }
  proxy_ = proxy;
  cascade_ = cascade;
  if (proxy_) {
    oid_ = proxy_->id();
    if (is_internal() && is_inserted_ && proxy_->ostore_) {
      ++(*proxy_);
      if (relation_info_ && notify_foreign_relation) {
        relation_info_->insert_value_into_foreign(owner_, proxy_);
      }
    }
    proxy_->add(this);
  }
}

}