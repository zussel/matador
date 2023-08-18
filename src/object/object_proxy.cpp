#include "matador/object/object_store.hpp"
#include "matador/object/update_action.hpp"
#include "matador/object/object_proxy.hpp"


using namespace std;

namespace matador {

object_proxy::object_proxy(const identifier &pk)
  : pk_(pk)
{}

object_proxy::~object_proxy()
{
  if (object_type_entry_->store() && id() > 0) {
    // Todo: callback to object store?
//    store_->delete_proxy(id());
  }
  if (obj_) {
    deleter_(obj_);
  }
  for (auto &ptr : ptr_set_) {
    ptr->proxy_ = nullptr;
  }
}

const char *object_proxy::classname() const
{
  return name_();
}

object_store *object_proxy::ostore() const
{
  return object_type_entry_->store();
}

prototype_node *object_proxy::node() const
{
  return object_type_entry_->node();
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

void object_proxy::restore(byte_buffer &buffer, object_deserializer &deserializer)
{
  if ( obj_ == nullptr ) {
    create_object();
  }
  object_type_entry_->restore(this, buffer, deserializer);
}

void object_proxy::backup(byte_buffer &buffer, object_serializer &serializer)
{
  object_type_entry_->backup(this, buffer, serializer);
}

void object_proxy::insert_object()
{
  object_type_entry_->insert_object(this);
}

void object_proxy::delete_object()
{
  object_type_entry_->delete_object(this);
}

void object_proxy::mark_modified()
{
  object_type_entry_->mark_modified(this);
}

void object_proxy::sync_id()
{
  object_type_entry_->sync_id(this);
}

const std::type_index& object_proxy::type_index() const
{
  return object_type_entry_->type_index();
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
  return object_type_entry_->store() && prev_ && next_;
}

unsigned long long object_proxy::id() const {
  return oid;
}

void object_proxy::id(unsigned long long i)
{
  oid = i;
}

bool object_proxy::has_identifier() const
{
  return !pk_.is_null();
}

const identifier& object_proxy::pk() const
{
  return pk_;
}

identifier& object_proxy::pk()
{
  return pk_;
}

void object_proxy::pk(const identifier &id)
{
  pk_ = id;
}

void object_proxy::create_object()
{
  creator_(this);
}

std::ostream& operator <<(std::ostream &os, const object_proxy &op)
{
  os << "proxy [" << &op << "] (oid: " << op.oid << ", type: " << (op.object_type_entry_->node() ? op.object_type_entry_->node()->type() : op.classname()) << ")"
     << " prev_ [" << op.prev_ << "]"
     << " next_ [" << op.next_ << "] object [" << op.obj_ << "] "
     << " refs [" << op.reference_counter_ << "]";
  return os;
}

}
