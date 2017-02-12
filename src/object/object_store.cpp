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

#include "oos/object/object_store.hpp"

#include <iomanip>

using namespace std;
using namespace std::placeholders;

namespace oos {

object_store::object_store()
  : first_(new prototype_node)
  , last_(new prototype_node)
  , object_inserter_(*this)
{
  // empty tree where first points to last and
  // last points to first sentinel
  first_->next = last_;
  last_->prev = first_;
}

object_store::~object_store()
{
  clear(true);
  delete last_;
  delete first_;
}

void object_store::detach(const char *type)
{
  prototype_node *node = find_prototype_node(type);
  if (!node) {
    throw object_exception("unknown prototype type");
  }

  node->on_detach();

  remove_prototype_node(node, node->depth == 0);
}

object_store::iterator object_store::detach(const prototype_iterator &i)
{
  if (i == end() || i.get() == nullptr) {
    throw object_exception("invalid prototype iterator");
  }

  i->on_detach();

  return remove_prototype_node(i.get(), i->depth == 0);
}

object_store::iterator object_store::find(const char *type)
{
  prototype_node *node = find_prototype_node(type);
  if (!node) {
    return end();
  }
  return iterator(node);
}

object_store::const_iterator object_store::find(const char *type) const
{
  prototype_node *node = find_prototype_node(type);
  if (!node) {
    return end();
  }
  return const_iterator(node);
}

object_store::const_iterator object_store::begin() const
{
  return const_iterator(first_->next);
}

prototype_iterator object_store::begin() {
  return prototype_iterator(first_->next);
}

const_prototype_iterator object_store::end() const
{
  return const_iterator(last_);
}

prototype_iterator object_store::end()
{
  return iterator(last_);
}

void object_store::clear(bool full)
{
  if (full) {
    while (first_->next != last_) {
      remove_prototype_node(first_->next, true);
    }
  } else {
    // only delete objects
    prototype_iterator first = begin();
    prototype_iterator last = end();
    while (first != last) {
        (first++)->clear(false);
    }
  }
  object_map_.clear();
}

void object_store::clear(const char *type)
{
  clear(find(type));
}

void object_store::clear(const prototype_iterator &node)
{
  clear(node.get());
}

prototype_node* object_store::clear(prototype_node *node)
{
  prototype_node *current = node->first->next;
  while (current != node->last.get()) {
    current = clear(current);
  }
  // finally link first to last and vice versa
  return remove_prototype_node(node, false);
}

size_t object_store::size() const
{
  return (size_t) (std::distance(begin(), end()));
}

bool object_store::empty() const
{
  bool is_empty = true;
  for_each_root_node([&](const_prototype_iterator i) {
    is_empty &= i->empty(false);
  });
  return is_empty;
}

size_t object_store::depth(const prototype_node *node) const
{
  size_t d = 0;
  while (node->parent) {
    node = node->parent;
    ++d;
  }
  return d;
}

void object_store::dump(std::ostream &out) const
{
  const_prototype_iterator node = begin();
  out << "digraph G {\n";
  out << "\tgraph [fontsize=10]\n";
  out << "\tnode [color=\"#0c0c0c\", fillcolor=\"#dd5555\", shape=record, style=\"rounded,filled\", fontname=\"Verdana-Bold\"]\n";
  out << "\tedge [color=\"#0c0c0c\"]\n";
  do {
    size_t d = depth(node.get());
    for (size_t i = 0; i < d; ++i) out << " ";
    out << *node;
    out.flush();
  } while (++node != end());
  out << "}" << std::endl;

  out << "prototype map item keys\n";
  std::for_each(prototype_map_.begin(), prototype_map_.end(), [&](const t_prototype_map::value_type &item) {
    out << "key: " << item.first << "\n";
  });
  out << "typeid map item keys\n";
  std::for_each(typeid_prototype_map_.begin(), typeid_prototype_map_.end(), [&](const t_typeid_prototype_map::value_type &item) {
    out << "key: " << item.first << "\n";
  });
}

void object_store::dump_objects(std::ostream &out) const
{
  const_prototype_iterator root = begin();
  out << "dumping all objects\n";

  object_proxy *op = root->op_first;
  while (op) {
    out << "[" << op << "] (";
    if (op->obj()) {
      out << op->obj() << " prev [" << op->prev()->obj() << "] next [" << op->next()->obj() << "])\n";
    } else {
      out << "serializable 0)\n";
    }
    op = op->next_;
  }
}

object_proxy* object_store::find_proxy(unsigned long id) const
{
  t_object_proxy_map::const_iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    return nullptr;
  } else {
    return i->second;
  }
}

bool object_store::delete_proxy(unsigned long id)
{
  t_object_proxy_map::iterator i = object_map_.find(id);
  if (i == object_map_.end()) {
    return false;
  } else if (i->second->linked()) {
    return false;
  } else {
    object_map_.erase(i);
    return true;
  }
}

object_proxy* object_store::insert_proxy(object_proxy *proxy)
{
  if (proxy == nullptr) {
    throw object_exception("proxy is null");
  }
  if (proxy->obj() == nullptr) {
    throw object_exception("object is null");
  }
  if (proxy->id() == 0) {
    throw_object_exception("object proxy doesn't have an id");
  }
  iterator node = find(proxy->classname());
  if (node == end()) {
    throw object_exception("couldn't find object type");
  }

  node->insert(proxy);

  return object_map_.insert(std::make_pair(proxy->id(), proxy)).first->second;
}

void object_store::remove_proxy(object_proxy *proxy)
{
  if (proxy == nullptr) {
    throw object_exception("object proxy is nullptr");
  }
  if (proxy->node() == nullptr) {
    throw object_exception("prototype node is nullptr");
  }
  // single deletion
  if (object_map_.erase(proxy->id()) != 1) {
    // couldn't remove object
    // throw exception
    throw object_exception("couldn't remove object");
  }

  proxy->node()->remove(proxy);
  delete proxy;
}

object_proxy* object_store::register_proxy(object_proxy *oproxy)
{
  if (oproxy->id() != 0) {
    throw_object_exception("object proxy already registerd");
  }

  if (oproxy->node() == nullptr) {
    throw_object_exception("object proxy hasn't git a prototype node");
  }

  oproxy->id(seq_.next());

  return object_map_.insert(std::make_pair(oproxy->id(), oproxy)).first->second;
}

sequencer_impl_ptr object_store::exchange_sequencer(const sequencer_impl_ptr &seq)
{
  return seq_.exchange_sequencer(seq);
}

void object_store::on_update_relation_owner(prototype_node::relation_info &info, object_proxy *owner, object_proxy *value)
{
  if (!is_relation_notification_enabled()) {
    return;
  }
  disable_relation_notification();
  info.insert_value(owner, info.name, value);
  enable_relation_notification();
}

void object_store::on_remove_relation_owner(prototype_node::relation_info &info, object_proxy *owner, object_proxy *value)
{
  if (!is_relation_notification_enabled()) {
    return;
  }
  disable_relation_notification();
  info.remove_value(owner, info.name, value);
  enable_relation_notification();
}

void object_store::on_append_relation_item(prototype_node &node, object_proxy *owner, object_proxy *value)
{
  auto i = node.relation_info_map_.find(node.type_index());
  if (i == node.relation_info_map_.end()) {
    return;
  }
  on_update_relation_owner(i->second, owner, value);
}

void object_store::on_remove_relation_item(prototype_node &node, object_proxy *owner, object_proxy *value)
{
  auto i = node.relation_info_map_.find(node.type_index());
  if (i == node.relation_info_map_.end()) {
    return;
  }
  on_remove_relation_owner(i->second, owner, value);
}

prototype_node* object_store::find_prototype_node(const char *type) const {
  // check for null
  if (type == 0) {
    throw object_exception("invalid type (null)");
  }
  /*
   * first search in the prototype map
   */
  t_prototype_map::const_iterator i = prototype_map_.find(type);
  if (i == prototype_map_.end()) {
    /*
   * if not found search in the typeid to prototype map
   */
    t_typeid_prototype_map::const_iterator j = typeid_prototype_map_.find(type);
    if (j == typeid_prototype_map_.end()) {
      return nullptr;
    } else {
      const t_prototype_map &val = j->second;
      /*
     * if size is greater one (1) the name
     * is a typeid and has more than one prototype
     * node and therefor it is not unique and an
     * exception is thrown
     */
      if (val.size() > 1) {
        // throw exception
        throw object_exception("type id not unique");
      } else {
        // return the only prototype
        return val.begin()->second;
      }
    }
  } else {
    return i->second;
  }
}

prototype_node* object_store::remove_prototype_node(prototype_node *node, bool is_root)
{
  // remove (and delete) from tree (deletes subsequently all child nodes
  // for each child call remove_prototype(child);
  prototype_node *next = node->next_node(node);

  while (node->has_children()) {
    remove_prototype_node(node->first->next, false);
  }
  // and objects they're containing
  node->clear(false);
  // delete prototype node as well
  // unlink node
  node->unlink();
  // get iterator
  t_prototype_map::iterator j = prototype_map_.find(node->type_.c_str());
  if (j != prototype_map_.end()) {
    prototype_map_.erase(j);
  }
  // find item in typeid map
  t_typeid_prototype_map::iterator k = typeid_prototype_map_.find(node->type_id());
  if (k != typeid_prototype_map_.end()) {
    k->second.erase(node->type_);
    if (k->second.empty()) {
      typeid_prototype_map_.erase(k);
    }
  } else {
    throw object_exception("couldn't find node by id");
  }
  if (is_root) {
    delete node->op_first;
    delete node->op_last;
  }
  delete node;
  return next;
}

prototype_node *object_store::find_parent(const char *name) const
{
  prototype_node *parent_node(nullptr);
  if (name != nullptr) {
    parent_node = find_prototype_node(name);
    if (!parent_node) {
      throw object_exception("unknown prototype type");
    }
  }
  return parent_node;
}

void object_store::push_transaction(const transaction &tr)
{
  transactions_.push(tr);
}

void object_store::pop_transaction()
{
  transactions_.pop();
}

bool object_store::is_relation_notification_enabled()
{
  return relation_notification_;
}

void object_store::enable_relation_notification()
{
  relation_notification_ = true;
}

void object_store::disable_relation_notification()
{
  relation_notification_ = false;
}

transaction object_store::current_transaction()
{
  return transactions_.top();
}

bool object_store::has_transaction() const
{
  return !transactions_.empty();
}


}
