/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "database/result.hpp"
#include "database/statement.hpp"

#include "object/serializable.hpp"
#include "object/object_ptr.hpp"
#include "object/object_exception.hpp"
#include "object/prototype_tree.hpp"

namespace oos {


result_iterator::result_iterator() {

}

result_iterator::result_iterator(const result_iterator &x) {

}

result_iterator &result_iterator::operator=(const result_iterator &x) {
  return <#initializer#>;
}

result_iterator::~result_iterator() {

}

bool result_iterator::operator==(const result_iterator &rhs) {
  return false;
}

bool result_iterator::operator!=(const result_iterator &rhs) {
  return false;
}

result_iterator &result_iterator::operator++() {
  return <#initializer#>;
}

result_iterator result_iterator::operator++(int i) {
  return oos::result_iterator();
}

result_iterator::value_type &result_iterator::operator*() {
  return <#initializer#>;
}

result_iterator::pointer result_iterator::operator->() {
  return nullptr;
}

result_iterator::pointer result_iterator::operator&() {
  return nullptr;
}


result::result() {

}

result::~result() {

}

result::iterator result::begin() {
  return oos::result_iterator();
}

result::iterator result::end() {
  return oos::result_iterator();
}

bool result::empty() const {
  return false;
}

std::size_t result::size() const {
  return 0;
}

result_impl::result_impl()
{}

result_impl::~result_impl()
{}

void result_impl::get(serializable *o)
{
  result_index = transform_index(0);
  o->deserialize(*this);
}

serializable* result_impl::fetch(const oos::prototype_node *node)
{
  node_ = node;

  std::unique_ptr<serializable> obj(node_->producer->create());

  if (!fetch(obj.get())) {
    obj.reset(nullptr);
  }

  node_ = nullptr;
  return obj.release();
}

void result_impl::read(const char *id, object_base_ptr &x)
{
  /*
   * read key
   * if valid key
   * create serializable with key
   * insert into object store
   * reset object ptr
   */
  // find prototype node of foreign key
  prototype_node::t_foreign_key_map::const_iterator i = node_->foreign_keys.find(id);
  if (i == node_->foreign_keys.end()) {
    throw_object_exception("couldn't find foreign key for serializable of type'" << x.type() << "'");
  }

  /*
   * clone new primary key and deserialize it
   * if valid value is set create new proxy
   * with primary key
   */
  std::shared_ptr<primary_key_base> pk(i->second->clone());
  pk->deserialize(id, *this);
  if (!pk->is_valid()) {
    return;
  }

  // get node of object type
  prototype_iterator xnode = node_->tree->find(x.type());

  std::unique_ptr<object_proxy> proxy(xnode->find_proxy(pk));

  if (!proxy) {
    proxy.reset(new object_proxy(pk, const_cast<prototype_node*>(xnode.get())));
  }

  x.reset(proxy.release());
}

void result_impl::read(const char */*id*/, object_container &/*x*/) { }

void result_impl::read(const char *id, primary_key_base &x)
{
  x.deserialize(id, *this);
}

const prototype_node* result_impl::node() const
{
  return node_;
}


}
