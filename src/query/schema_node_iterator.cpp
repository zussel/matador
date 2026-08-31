#include "matador/query/schema_node_iterator.hpp"
#include "matador/query/schema_node.hpp"

namespace matador::query {
schema_node_iterator::schema_node_iterator(pointer node)
: node_(node) {
}

bool schema_node_iterator::operator==(const schema_node_iterator &i) const {
  return (node_ == i.node_);
}

bool schema_node_iterator::operator!=(const schema_node_iterator &i) const {
  return !operator==(i);
}

schema_node_iterator & schema_node_iterator::operator++() {
  increment();
  return *this;
}

schema_node_iterator schema_node_iterator::operator++(int) {
  pointer tmp = node_;
  increment();
  return schema_node_iterator(tmp);
}

schema_node_iterator & schema_node_iterator::operator--() {
  decrement();
  return *this;
}

schema_node_iterator schema_node_iterator::operator--(int) {
  pointer tmp = node_;
  decrement();
  return schema_node_iterator(tmp);
}

schema_node_iterator::pointer schema_node_iterator::operator->() const {
  return node_;
}

schema_node_iterator::reference schema_node_iterator::operator*() const {
  return *node_;
}

schema_node_iterator::pointer schema_node_iterator::get() const {
  return node_;
}

void schema_node_iterator::increment() {
  if (!node_) {
    return;
  }

  node_ = node_->next();
}

void schema_node_iterator::decrement() {
  if (!node_) {
    return;
  }

  node_ = node_->prev();
}

const_schema_node_iterator::const_schema_node_iterator(pointer node)
: node_(node) {
}

bool const_schema_node_iterator::operator==(const const_schema_node_iterator &i) const {
  return (node_ == i.node_);
}

bool const_schema_node_iterator::operator!=(const const_schema_node_iterator &i) const {
  return !operator==(i);
}

const_schema_node_iterator &const_schema_node_iterator::operator++() {
  increment();
  return *this;
}

const_schema_node_iterator const_schema_node_iterator::operator++(int) {
  pointer tmp = node_;
  increment();
  return const_schema_node_iterator(tmp);
}

const_schema_node_iterator &const_schema_node_iterator::operator--() {
  decrement();
  return *this;
}

const_schema_node_iterator const_schema_node_iterator::operator--(int) {
  pointer tmp = node_;
  decrement();
  return const_schema_node_iterator(tmp);
}

const_schema_node_iterator::pointer const_schema_node_iterator::operator->() const {
  return node_;
}

const_schema_node_iterator::reference const_schema_node_iterator::operator*() const {
  return *node_;
}

const_schema_node_iterator::pointer const_schema_node_iterator::get() const {
  return node_;
}

void const_schema_node_iterator::increment() {
  if (!node_) {
    return;
  }

  node_ = node_->next();
}

void const_schema_node_iterator::decrement() {
  if (!node_) {
    return;
  }

  node_ = node_->prev();
}
}
