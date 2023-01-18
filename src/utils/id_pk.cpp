#include "matador/utils/id_pk.hpp"

namespace matador {

id_pk::base::base(const std::type_index &ti)
  : type_index_(ti) {}

bool id_pk::base::is_same_type(const base &x) const {
  return type_index_ == x.type_index_;
}

id_pk::null_pk::null_pk()
  : base(std::type_index(typeid(null_type_t)))
{}

id_pk::base* id_pk::null_pk::copy() const
{
  return new null_pk;
}

bool id_pk::null_pk::equal_to(const id_pk::base &x) const
{
  return is_same_type(x);
}

bool id_pk::null_pk::less(const id_pk::base &x) const
{
  return is_same_type(x);
}

std::string id_pk::null_pk::str() const
{
  return "null_pk";
}

id_pk::id_pk()
  : id_(std::make_unique<null_pk>()) {}

id_pk::id_pk(const id_pk &x)
  : id_(x.id_->copy()) {}

id_pk &id_pk::operator=(const id_pk &x) {
  id_.reset(x.id_->copy());
  return *this;
}

id_pk::id_pk(id_pk &x)
  : id_(std::move(x.id_)) {}

id_pk &id_pk::operator=(id_pk &&x) noexcept {
  id_ = std::move(x.id_);
  return *this;
}

bool id_pk::operator==(const id_pk &x) const {
  return id_->equal_to(*x.id_);
}

bool id_pk::operator!=(const id_pk &x) const {
  return !operator==(x);
}

bool id_pk::operator<(const id_pk &x) const {
  return id_->less(*x.id_);
}

bool id_pk::operator<=(const id_pk &x) const {
  return operator==(x) || operator<(x);
}

bool id_pk::operator>(const id_pk &x) const {
  return !operator<=(x);
}

bool id_pk::operator>=(const id_pk &x) const {
  return !operator<(x);
}

bool id_pk::is_same_type(const id_pk &x) const {
  return id_->is_same_type(*x.id_);
}

std::string id_pk::str() const {
  return id_->str();
}

const std::type_index &id_pk::type_index() const {
  return id_->type_index_;
}

std::ostream &operator<<(std::ostream &out, const id_pk &id) {
  out << id.str();
  return out;
}
}