#include "matador/utils/identifier.hpp"

#include <stdexcept>
#include <ostream>

namespace matador {
namespace detail {
std::string to_string(const std::string &value) {
  return value;
}

bool is_valid(const std::string &value) {
  return !value.empty();
}
}

identifier::base::base(const std::type_index &ti)
  : type_index_(ti) {}

bool identifier::base::is_same_type(const base &x) const {
  return type_index_ == x.type_index_;
}

identifier::null_pk::null_pk()
  : base(std::type_index(typeid(null_type_t)))
{}

identifier::base* identifier::null_pk::copy() const
{
  return new null_pk;
}

bool identifier::null_pk::equal_to(const identifier::base &x) const
{
  return is_same_type(x);
}

bool identifier::null_pk::less(const identifier::base &x) const
{
  return is_same_type(x);
}

bool identifier::null_pk::is_valid() const
{
  return false;
}

std::string identifier::null_pk::str() const
{
  return "null_pk";
}

void identifier::null_pk::serialize(identifier_serializer &s)
{
  s.serialize(null_);
}

size_t identifier::null_pk::hash() const
{
  throw std::runtime_error("hash for null_pk not allowed");
}

identifier::identifier()
  : id_(std::make_unique<null_pk>()) {}

identifier::identifier(const identifier &x)
  : id_(x.id_->copy()) {}

identifier &identifier::operator=(const identifier &x) {
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

bool identifier::is_same_type(const identifier &x) const {
  return id_->is_same_type(*x.id_);
}

std::string identifier::str() const {
  return id_->str();
}

const std::type_index &identifier::type_index() const {
  return id_->type_index_;
}

bool identifier::is_null() const {
  return is_same_type(null_identifier);
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

std::ostream &operator<<(std::ostream &out, const identifier &id) {
  out << id.str();
  return out;
}

size_t id_pk_hash::operator()(const identifier &id) const
{
  return id.hash();
}

}