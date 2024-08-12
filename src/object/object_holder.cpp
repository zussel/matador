#include "matador/object/object_holder.hpp"
#include "matador/object/object_proxy.hpp"
#include "matador/object/object_store.hpp"
#include "matador/object/object_exception.hpp"

namespace matador {

object_holder::object_holder(const object_holder &x)
{
  reset(x.proxy_, x.attributes_);
}

object_holder::object_holder(object_holder &&x) noexcept
{
  object_proxy *proxy = x.proxy_;
  if (proxy) {
    proxy->remove(&x);
    if (x.is_internal() && x.is_inserted_ && proxy->object_type_entry_->store()) {
      --(*proxy);
    }
  }
  x.proxy_ = nullptr;
  reset(proxy, x.attributes_, true);
  attributes_ = x.attributes_;
  x.owner_ = nullptr;
}

object_holder &
object_holder::operator=(const object_holder &x)
{
  if (this != &x && proxy_ != x.proxy_) {
    relation_info_ = x.relation_info_;
    reset(x.proxy_, utils::cascade_type::NONE);
  }
  return *this;
}

object_holder &object_holder::operator=(object_holder &&x) noexcept
{
  if (this != &x) {
    object_proxy *proxy = x.proxy_;
    if (proxy) {
      proxy->remove(&x);
      if (x.is_internal() && x.is_inserted_ && proxy->object_type_entry_->store()) {
        --(*proxy);
      }
    }
    reset(proxy, x.attributes_, true);
    attributes_ = x.attributes_;
    x.owner_ = nullptr;
  }
  return *this;
}

object_holder::object_holder(object_proxy *proxy)
  : proxy_(proxy)
{
  if (proxy_) {
    proxy_->add(this);
  }
}

object_holder::~object_holder()
{
  if (proxy_) {
    if (is_internal() && is_inserted_ && relation_info_) {
      --(*proxy_);
    }
    proxy_->remove(this);
    /*
     * if proxy was created temporary
     * we can delete it here
     */
    if (!proxy_->is_inserted() && proxy_->is_isolated()) {
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

object_holder::operator bool() const noexcept
{
  return proxy_ != nullptr;
}

void object_holder::reset(object_proxy *proxy, const utils::foreign_attributes &attr)
{
  reset(proxy, attr, true);
}

void object_holder::reset(object_holder &holder)
{
  reset(holder.proxy_, holder.attributes_);
}

void object_holder::clear()
{
  reset(nullptr, utils::cascade_type::ALL);
}

bool object_holder::empty() const noexcept
{
  return proxy_ == nullptr;
}

bool object_holder::valid() const noexcept
{
  return !empty();
}

void object_holder::reset(const identifier &id)
{
  if (proxy_ && !proxy_->pk().is_similar_type(id)) {
    throw object_exception("identifier types are not equal");
  }
  reset(new object_proxy(id), utils::cascade_type::NONE);
}

bool object_holder::is_loaded() const
{
  return (proxy_ && proxy_->obj());
}

unsigned long long object_holder::id() const
{
  return (proxy_ ? proxy_->id() : 0);
}

//object_store *object_holder::store() const
//{
//  return (proxy_ ? proxy_->store() : nullptr);
//}

void* object_holder::ptr()
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
    return proxy_->obj();
  } else {
    return nullptr;
  }
}

void*object_holder::lookup_object() const
{
  return proxy_ ? proxy_->obj() : nullptr;
}

bool object_holder::is_internal() const
{
  return proxy_->is_inserted();
}

bool object_holder::is_inserted() const
{
  return is_inserted_;
}

bool object_holder::has_primary_key() const
{
  return proxy_ != nullptr && proxy_->has_identifier();
}

const identifier& object_holder::primary_key() const
{
  return (proxy_ ? proxy_->pk() : null_identifier);
}

identifier& object_holder::primary_key()
{
  return (proxy_ ? proxy_->pk() : null_identifier);
}

unsigned long object_holder::reference_count() const
{
  return (proxy_ ? proxy_->reference_count() : 0UL);
}

utils::cascade_type object_holder::cascade() const
{
  return attributes_.cascade();
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

void object_holder::reset(object_proxy *proxy, const utils::foreign_attributes &attr, bool notify_foreign_relation)
{
  if (proxy_ == proxy) {
    return;
  }
  if (proxy_) {
    proxy_->remove(this);
    if (is_internal() && is_inserted_ && proxy_->is_inserted()) {
      --(*proxy_);
      if (relation_info_ && notify_foreign_relation) {
        relation_info_->remove_value_from_foreign(owner_, proxy_);
      }
    }
    /*
     * if proxy was created temporary
     * we can delete it here
     */
    if (proxy_ && !proxy_->is_inserted() && proxy_->ptr_set_.empty()) {
      delete proxy_;
    }
  }
  proxy_ = proxy;
  attributes_ = attr;
  if (proxy_) {
    if (is_internal() && is_inserted_ && proxy_->is_inserted()) {
      ++(*proxy_);
      if (relation_info_ && notify_foreign_relation) {
        relation_info_->insert_value_into_foreign(owner_, proxy_);
      }
    }
    proxy_->add(this);
  }
}

}