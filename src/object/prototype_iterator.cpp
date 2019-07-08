//
// Created by sascha on 11/30/15.
//
#include "matador/object/prototype_iterator.hpp"

namespace matador {

prototype_iterator::prototype_iterator(prototype_iterator::pointer node)
  : node_(node)
{}

prototype_iterator::prototype_iterator(const prototype_iterator &x)
  : node_(x.node_)
{}

prototype_iterator& prototype_iterator::operator=(const prototype_iterator &x)
{
  node_ = x.node_;
  return *this;
}

bool prototype_iterator::operator==(const prototype_iterator &i) const
{
  return (node_ == i.node_);
}

bool prototype_iterator::operator==(const const_prototype_iterator &i) const
{
  return node_ == i.get();
}

bool prototype_iterator::operator!=(const prototype_iterator &i) const
{
  return !operator==(i);
}


bool prototype_iterator::operator!=(const const_prototype_iterator &i) const
{
  return !operator==(i);
}

prototype_iterator::self& prototype_iterator::operator++()
{
  increment();
  return *this;
}

prototype_iterator::self prototype_iterator::operator++(int)
{
  prototype_node *tmp = node_;
  increment();
  return prototype_iterator(tmp);
}

prototype_iterator::self& prototype_iterator::operator--()
{
  decrement();
  return *this;
}

prototype_iterator::self prototype_iterator::operator--(int)
{
  prototype_node *tmp = node_;
  decrement();
  return prototype_iterator(tmp);
}

prototype_iterator::pointer prototype_iterator::operator->() const
{
  return node_;
}

prototype_iterator::reference prototype_iterator::operator*() const
{
  return *node_;
}

prototype_iterator::pointer prototype_iterator::get() const
{
  return node_;
}

void prototype_iterator::increment()
{
  if (node_) {
    node_ = node_->next_node();
  }
}
void prototype_iterator::decrement()
{
  if (node_) {
    node_ = node_->previous_node();
  }
}

const_prototype_iterator::const_prototype_iterator()
  : node_(0)
{}

const_prototype_iterator::const_prototype_iterator(const_prototype_iterator::pointer node)
  : node_(node)
{}

const_prototype_iterator::const_prototype_iterator(const const_prototype_iterator &x)
  : node_(x.node_)
{}

const_prototype_iterator::const_prototype_iterator(const prototype_iterator &x)
  : node_(x.node_)
{}

const_prototype_iterator& const_prototype_iterator::operator=(const const_prototype_iterator &x)
{
  node_ = x.node_;
  return *this;
}

const_prototype_iterator& const_prototype_iterator::operator=(const prototype_iterator &x)
{
  node_ = x.node_;
  return *this;
}

bool const_prototype_iterator::operator==(const const_prototype_iterator &i) const
{
  return (node_ == i.node_);
}

bool const_prototype_iterator::operator!=(const const_prototype_iterator &i) const
{
  return !operator==(i);
}

const_prototype_iterator::self& const_prototype_iterator::operator++()
{
  increment();
  return *this;
}

const_prototype_iterator::self const_prototype_iterator::operator++(int)
{
  pointer tmp = node_;
  increment();
  return const_prototype_iterator(tmp);
}

const_prototype_iterator::self& const_prototype_iterator::operator--()
{
  decrement();
  return *this;
}

const_prototype_iterator::self const_prototype_iterator::operator--(int)
{
  pointer tmp = node_;
  decrement();
  return const_prototype_iterator(tmp);
}

const_prototype_iterator::pointer const_prototype_iterator::operator->() const
{
  return node_;
}

const_prototype_iterator::reference const_prototype_iterator::operator*() const
{
  return *node_;
}

const_prototype_iterator::pointer const_prototype_iterator::get() const
{
  return node_;
}

void const_prototype_iterator::increment()
{
  if (node_) {
    node_ = node_->next_node();
  }
}
void const_prototype_iterator::decrement()
{
  if (node_) {
    node_ = node_->previous_node();
  }
}

}