#include "matador/object/object_store.hpp"
#include "matador/object/update_action.hpp"

using namespace std;

namespace matador {

template<class T>
update_action* object_proxy::create_update_action_internal(object_proxy *proxy)
{
  return new update_action(proxy->classname(), proxy->id(), proxy, static_cast<T*>(proxy->obj));
}

object_proxy::object_proxy(basic_identifier *pk)
  : primary_key_(pk)
{}

object_proxy::~object_proxy()
{
  if (ostore_ && id() > 0) {
    // Todo: callback to object store?
//    store_->delete_proxy(id());
  }
  if (obj_) {
    deleter_(obj_);
  }
  ostore_ = nullptr;
  for (auto &ptr : ptr_set_) {
    ptr->proxy_ = nullptr;
  }
}

const char *object_proxy::classname() const
{
  return namer_();
}

object_store *object_proxy::ostore() const
{
  return ostore_;
}

prototype_node *object_proxy::node() const
{
  return node_;
}

void object_proxy::link(object_proxy *successor)
{
  // link serializable proxy before this node_
  prev_ = successor->prev_;
  next_ = successor;
  if (successor->prev_) {
    successor->prev_->next_ = this;
  }
  successor->prev_ = this;
}

void object_proxy::unlink()
{
  if (prev_) {
    prev_->next_ = next_;
  }
  if (next_) {
    next_->prev_ = prev_;
  }
  prev_ = nullptr;
  next_ = nullptr;
  node_ = nullptr;
}

unsigned long object_proxy::operator++()
{
  return ++reference_counter_;
}

unsigned long object_proxy::operator++(int)
{
  return reference_counter_++;
}

unsigned long object_proxy::operator--()
{
  return --reference_counter_;
}

unsigned long object_proxy::operator--(int)
{
  return reference_counter_--;
}

bool object_proxy::linked() const
{
  return node_ != nullptr;
}

object_proxy *object_proxy::next() const
{
  return next_;
}

object_proxy *object_proxy::prev() const
{
  return prev_;
}

unsigned long object_proxy::reference_count() const
{
  return reference_counter_;
}

void object_proxy::restore(byte_buffer &buffer, object_store *store, object_serializer &serializer)
{
  if ( obj_ == nullptr ) {
    create_object();
  }
  restore_func
}

void object_proxy::add(object_holder *ptr)
{
  ptr_set_.insert(ptr);
}

bool object_proxy::remove(object_holder *ptr)
{
  return ptr_set_.erase(ptr) == 1;
}

bool object_proxy::valid() const
{
  return ostore_ && node_ && prev_ && next_;
}

unsigned long object_proxy::id() const {
  return oid;
}

void object_proxy::id(unsigned long i)
{
  oid = i;
}

bool object_proxy::has_identifier() const
{
  return primary_key_ != nullptr;
}

basic_identifier* object_proxy::pk() const
{
  return primary_key_;
}

void object_proxy::pk(basic_identifier *id)
{
  primary_key_ = id;
}

update_action* object_proxy::create_update_action()
{
  return create_update_action_func_(this);
}

void object_proxy::backup(byte_buffer &buffer, object_serializer &serializer) {
  backup_func_(this, buffer, serializer);
}

transaction object_proxy::current_transaction()
{
  return ostore_->current_transaction();
}

bool object_proxy::has_transaction() const
{
  return ostore_->has_transaction();
}

void object_proxy::create_object()
{
  creator_(this);
}

std::ostream& operator <<(std::ostream &os, const object_proxy &op)
{
  os << "proxy [" << &op << "] (oid: " << op.oid << ", type: " << (op.node_ ? op.node_->type() : op.classname()) << ")"
     << " prev_ [" << op.prev_ << "]"
     << " next_ [" << op.next_ << "] object [" << op.obj_ << "] "
     << " refs [" << op.reference_counter_ << "]";
  return os;
}

}
