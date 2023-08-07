#include "matador/utils/identifier.hpp"

#include <stdexcept>
#include <ostream>

namespace matador {

identifier::base::base(const std::type_index &ti, detail::identifier_type id_type)
  : type_index_(ti)
  , identifier_type_(id_type)
{}

//bool identifier::base::is_same_type(const base &x) const {
//  return type_index_ == x.type_index_;
//}
//
//bool identifier::base::is_same_type(const std::type_index &x) const {
//  return type_index_ == x;
//}

bool identifier::base::is_similar_type(const identifier::base &x) const
{
  return identifier_type_ == x.type();
}

detail::identifier_type identifier::base::type() const
{
  return identifier_type_;
}

identifier::null_pk::null_pk()
  : base(std::type_index(typeid(null_type_t)), detail::identifier_type_traits<null_type_t>::type())
{}

identifier::base* identifier::null_pk::copy() const
{
  return new null_pk;
}

bool identifier::null_pk::equal_to(const identifier::base &x) const
{
//  return is_same_type(x);
  return type_index_ == x.type_index_;
}

bool identifier::null_pk::less(const identifier::base &x) const
{
//  return is_same_type(x);
  return type_index_ == x.type_index_;
}

bool identifier::null_pk::is_valid() const
{
  return detail::identifier_type_traits<null_type_t>::is_valid();
}

std::string identifier::null_pk::str() const
{
  return detail::identifier_type_traits<null_type_t>::to_string();
}

void identifier::null_pk::serialize(identifier_serializer &s)
{
  s.serialize(null_, -1);
}

size_t identifier::null_pk::hash() const
{
  throw std::runtime_error("hash for null_pk not allowed");
}

identifier::identifier()
  : id_(std::make_shared<null_pk>()) {}

identifier::identifier(const identifier &x)
  : id_(x.id_->copy()) {}

identifier &identifier::operator=(const identifier &x) {
  if (this == &x) {
    return *this;
  }
  id_.reset(x.id_->copy());
  return *this;
}

identifier::identifier(identifier &&x) noexcept
  : id_(std::move(x.id_)) {}

identifier &identifier::operator=(identifier &&x) noexcept {
  id_ = std::move(x.id_);
  return *this;
}

bool identifier::operator==(const identifier &x) const {
  return id_->equal_to(*x.id_);
}

bool identifier::operator!=(const identifier &x) const {
  return !operator==(x);
}

bool identifier::operator<(const identifier &x) const {
  return id_->less(*x.id_);
}

bool identifier::operator<=(const identifier &x) const {
  return operator==(x) || operator<(x);
}

bool identifier::operator>(const identifier &x) const {
  return !operator<=(x);
}

bool identifier::operator>=(const identifier &x) const {
  return !operator<(x);
}

//bool identifier::is_same_type(const identifier &x) const {
//  return id_->is_same_type(*x.id_);
//}

bool identifier::is_similar_type(const identifier &x) const
{
  return id_->is_similar_type(*x.id_);
}

std::string identifier::str() const {
  return id_->str();
}

const std::type_index &identifier::type_index() const {
  return id_->type_index_;
}

identifier identifier::share() const
{
  return identifier(id_);
}

size_t identifier::use_count() const
{
  return id_.use_count();
}

bool identifier::is_null() const
{
  return type_index() == null_identifier.type_index();
//  return is_same_type(null_identifier);
}

bool identifier::is_valid() const
{
  return id_->is_valid();
}

void identifier::clear()
{
  id_ = std::make_unique<null_pk>();
}

void identifier::serialize(identifier_serializer &s)
{
  id_->serialize(s);
}

size_t identifier::hash() const
{
  return id_->hash();
}

identifier::identifier(const std::shared_ptr<base> &id)
  : id_(id)
{}

std::ostream &operator<<(std::ostream &out, const identifier &id)
{
  out << id.str();
  return out;
}

size_t id_pk_hash::operator()(const identifier &id) const
{
  return id.hash();
}

}