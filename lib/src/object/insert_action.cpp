//
// Created by sascha on 12.03.16.
//

#include "object/insert_action.hpp"
#include "object/action_visitor.hpp"
#include "object/object_proxy.hpp"
#include "object/object_store.hpp"

#include <algorithm>

namespace oos {

void insert_action::accept(action_visitor *av)
{
  av->visit(this);
}

std::string insert_action::type() const
{
  return type_;
}

insert_action::iterator insert_action::begin()
{
  return object_proxy_list_.begin();
}

insert_action::const_iterator insert_action::begin() const
{
  return object_proxy_list_.begin();
}

insert_action::iterator insert_action::end()
{
  return object_proxy_list_.end();
}

insert_action::const_iterator insert_action::end() const
{
  return object_proxy_list_.end();
}

bool insert_action::empty() const
{
  return object_proxy_list_.empty();
}

struct object_by_id : public std::unary_function<unsigned long, bool>
{
  object_by_id(unsigned long id)
    : id_(id)
  {}
  bool operator()(const object_proxy *proxy) const
  {
    return (proxy->obj() && proxy->id() == id_);
  }

  unsigned long id_;
};

insert_action::iterator insert_action::find(unsigned long id)
{
  return std::find_if(object_proxy_list_.begin(), object_proxy_list_.end(), object_by_id(id));
}

insert_action::const_iterator insert_action::find(unsigned long id) const
{
  return std::find_if(object_proxy_list_.begin(), object_proxy_list_.end(), object_by_id(id));
}

void insert_action::push_back(object_proxy *proxy)
{
  object_proxy_list_.push_back(proxy);
}

insert_action::iterator insert_action::erase(insert_action::iterator i)
{
  return object_proxy_list_.erase(i);
}

void insert_action::restore(byte_buffer &, object_store *store)
{
  // remove objects from object store
  for (insert_action::iterator i = begin(); i != end(); ++i) {
    action::remove_proxy(*i, store);
  }
}

}
