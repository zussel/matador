#include "matador/object/prototype_node.hpp"
#include "matador/object/object_exception.hpp"
#include "matador/object/object_proxy.hpp"

#include <memory>
#include <algorithm>

using namespace std;

namespace matador {

void prototype_node::initialize(object_store *tree, const char *type, bool abstract)
{
  tree_ = tree;
  first = std::make_unique<prototype_node>();
  last = std::make_unique<prototype_node>();
  type_.assign(type);
  abstract_ = abstract;
  first->next = last.get();
  last->prev = first.get();
}

bool
prototype_node::empty(bool self) const
{
  return op_first->next() == (self ? op_marker : op_last);
}

unsigned long
prototype_node::size() const
{
  return count;
}

const char *prototype_node::type() const
{
  return type_.c_str();
}

const char *prototype_node::type_id() const
{
  return info_->type_index().name();
}

void prototype_node::append(prototype_node *sibling)
{
  sibling->parent = parent;

  sibling->prev = this;
  sibling->next = next;
  next = sibling;
  sibling->next->prev = sibling;
  sibling->depth = depth;

  if (!parent) {
    sibling->op_first = new object_proxy();
    sibling->op_last = sibling->op_marker = new object_proxy();
    sibling->op_first->next_ = sibling->op_last;
    sibling->op_last->prev_ = sibling->op_first;
  } else {
    throw object_exception("failed to add node as sibling: node has no parent");
    // 1. first
//    if (op_first->next() == op_last) {
//      // node hasn't any serializable (proxy)
//      sibling->op_first = op_first;
//    } else {
//      // node has some objects (proxy)
//      sibling->op_first = op_last->prev_;
//    }
//    // 2. marker
//    sibling->op_marker = op_last;
//    // 3. last
//    sibling->op_last = op_last;
  }
}

void prototype_node::insert(prototype_node *child)
{
  child->parent = this;
  child->prev = last->prev;
  child->next = last.get();
  last->prev->next = child;
  last->prev = child;
  // set depth
  child->depth = depth + 1;
  // set object proxy pointer
  // 1. first
  if (op_first->next() == op_last) {
    // node hasn't any serializable (proxy)
    child->op_first = op_first;
  } else {
    // node has some objects (proxy)
    child->op_first = op_last->prev_;
  }
  // 2. marker
  child->op_marker = op_last;
  // 3. last
  child->op_last = op_last;
}

void prototype_node::insert(object_proxy *proxy)
{
  // check count of serializable in subtree
  if (count >= 2) {

     // there are more than two objects (normal case)
     // insert before last last

    proxy->link(op_marker->prev_);
  } else if (count == 1) {

     // there is one serializable in subtree
     // insert as first; adjust "left" marker

    proxy->link(op_marker->prev_);
    adjust_left_marker(this, proxy->next_, proxy);
  } else { // node->count == 0

     // there is no serializable in subtree
     //insert as last; adjust "right" marker

    proxy->link(op_marker);
    adjust_left_marker(this, proxy->next_, proxy);
    adjust_right_marker(this, proxy->prev_, proxy);
  }
  // set prototype node
//  proxy->node_ = this;
  // adjust size
  ++count;
  // find and insert primary key
  if (proxy->primary_key_) {
    id_map_.insert(std::make_pair(proxy->primary_key_, proxy));
  }

  // notify observers
  on_insert_proxy(proxy);
}

void prototype_node::remove(object_proxy *proxy)
{
  if (proxy == op_first->next()) {
    // adjust left marker
    adjust_left_marker(this, op_first->next_, op_first->next_->next_);
  }
  if (proxy == op_marker->prev()) {
    // adjust right marker
    adjust_right_marker(this, proxy, op_marker->prev_->prev_);
  }
  // unlink object_proxy
  if (proxy->prev()) {
    proxy->prev_->next_ = proxy->next_;
  }
  if (proxy->next()) {
    proxy->next_->prev_ = proxy->prev_;
  }
  proxy->prev_ = nullptr;
  proxy->next_ = nullptr;

  if (has_primary_key()) {
    if (id_map_.erase(proxy->primary_key_) == 0) {
      // couldn't find and erase primary key
    }
  }

  // notify observers
  on_delete_proxy(proxy);

  // adjust serializable count for node
  --count;
}

void prototype_node::clear(bool recursive)
{
  if (!empty(true)) {
    adjust_left_marker(this, op_first->next_, op_marker);
    adjust_right_marker(this, op_marker->prev_, op_first);

    while (op_first->next() != op_marker) {
      object_proxy *op = op_first->next_;
      // remove serializable proxy from list
      op->unlink();
      // delete serializable proxy and serializable
      delete op;
    }
    id_map_.clear();
    count = 0;
  }

  if (recursive) {
    prototype_node *current = first->next;

    while (current != last.get()) {
      current->clear(recursive);
      current = current->next;
    }
  }
}

void prototype_node::unlink()
{
  // unlink node
  prev->next = next;
  next->prev = prev;
  next = nullptr;
  prev = nullptr;
}

prototype_node* prototype_node::next_node() const
{
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (first && first->next != last.get())
    return first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    const prototype_node *node = this;
    while (node->parent && node->next == node->parent->last.get()) {
      node = node->parent;
    }
    return node->next;
  }
}

prototype_node* prototype_node::next_node(const prototype_node *root) const
{
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (first && first->next != last.get())
    return first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    const prototype_node *node = this;
    while (node->parent && node->next == node->parent->last.get() && node->parent != root) {
      node = node->parent;
    }
    return node->next;
  }
}

prototype_node* prototype_node::previous_node() const
{
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last children. if so, we set the last last
  // child as our iterator
  if (prev && prev->prev) {
    const prototype_node *node = prev;
    while (node->last && node->first->next != node->last.get()) {
      node = node->last->prev;
    }
    return const_cast<prototype_node*>(node);
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    return parent;
  }
}

prototype_node* prototype_node::previous_node(const prototype_node *root) const
{
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last children. if so, we set the last last
  // child as our iterator
  if (prev && prev->prev) {
    const prototype_node *node = prev;
    while (node->last && node->first->next != node->last.get() && node->parent != root) {
      node = node->last->prev;
    }
    return const_cast<prototype_node*>(node);
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    return parent;
  }
}

object_store *prototype_node::tree() const
{
  return tree_;
}

bool prototype_node::is_child_of(const prototype_node *prnt) const
{
  const prototype_node *node = this;
  while (prnt->depth < node->depth) {
    node = node->parent;
  }
  return node == prnt;
}


bool prototype_node::has_children() const
{
  return first->next != last.get();
}

bool prototype_node::has_primary_key() const
{
  return id_ != nullptr;
}

basic_identifier *prototype_node::id() const
{
  return id_.get();
}

bool prototype_node::is_abstract() const
{
  return abstract_;
}

std::type_index prototype_node::type_index() const
{
  return info_->type_index();
}

object_proxy *prototype_node::find_proxy(basic_identifier *pk)
{
  auto i = std::find_if(id_map_.begin(), id_map_.end(), [pk](const detail::t_identifier_map::value_type &x) {
    return *pk == *(x.first);
  });
//  t_primary_key_map::iterator i = primary_key_map.find(pk);
  return (i != id_map_.end() ? i->second : nullptr);
}

void prototype_node::register_relation_endpoint(const std::type_index &tindex,
                                                const std::shared_ptr<detail::basic_relation_endpoint> &endpoint)
{
  info_->register_relation_endpoint(tindex, endpoint);
}

void prototype_node::unregister_relation_endpoint(const std::type_index &tindex)
{
  info_->unregister_relation_endpoint(tindex);
}

prototype_node::const_endpoint_iterator prototype_node::find_endpoint(const std::type_index &tindex) const
{
  return info_->find_relation_endpoint(tindex);
}

prototype_node::endpoint_iterator prototype_node::find_endpoint(const std::type_index &tindex)
{
  return info_->find_relation_endpoint(tindex);
}

prototype_node::const_endpoint_iterator prototype_node::find_endpoint(const std::string &field) const
{
  return info_->find_relation_endpoint(field);
}

prototype_node::endpoint_iterator prototype_node::find_endpoint(const std::string &field)
{
  return info_->find_relation_endpoint(field);
}

prototype_node::endpoint_iterator prototype_node::endpoint_begin()
{
  return info_->endpoint_begin();
}

prototype_node::const_endpoint_iterator prototype_node::endpoint_begin() const
{
  return info_->endpoint_begin();
}

prototype_node::endpoint_iterator prototype_node::endpoint_end()
{
  return info_->endpoint_end();
}

prototype_node::const_endpoint_iterator prototype_node::endpoint_end() const
{
  return info_->endpoint_end();
}

std::size_t prototype_node::endpoints_size() const
{
  return info_->endpoints_size();
}

bool prototype_node::endpoints_empty() const
{
  return info_->endpoints_empty();
}

const detail::abstract_prototype_info::t_endpoint_map &prototype_node::endpoints() const
{
  return info_->endpoints();
}

std::shared_ptr<detail::object_type_registry_entry_base> prototype_node::object_type_entry() const
{
  return object_type_entry_;
}

/*
 * adjust the marker of all predecessor nodes
 * self and last marker
 */
void prototype_node::adjust_left_marker(prototype_node *root, object_proxy *old_proxy, object_proxy *new_proxy)
{
  // store start node
  prototype_node *node = root->previous_node();
  // get previous node
  while (node) {
    if (node->op_marker == old_proxy) {
      node->op_marker = new_proxy;
    }
    if (node->depth >= root->depth && node->op_last == old_proxy) {
      node->op_last = new_proxy;
    }
    node = node->previous_node();
  }
}

void prototype_node::adjust_right_marker(prototype_node *root, object_proxy* old_proxy, object_proxy *new_proxy)
{
  // store start node
  prototype_node *node = root->next_node();
  while (node) {
    if (node->op_first == old_proxy) {
      node->op_first = new_proxy;
    }
    node = node->next_node();
  }
}

bool prototype_node::is_relation_node() const
{
  return is_relation_node_;
}

const prototype_node::relation_node_info &prototype_node::node_info() const
{
  return relation_node_info_;
}

void prototype_node::on_attach() const
{
  info_->notify(detail::notification_type::ATTACH, nullptr);
}

void prototype_node::on_detach() const
{
  info_->notify(detail::notification_type::DETACH, nullptr);
}

void prototype_node::on_insert_proxy(object_proxy *proxy) const
{
  info_->notify(detail::notification_type::INSERT, proxy);
}

void prototype_node::on_update_proxy(object_proxy *proxy) const
{
  info_->notify(detail::notification_type::UPDATE, proxy);
}

void prototype_node::on_delete_proxy(object_proxy *proxy) const
{
  info_->notify(detail::notification_type::REMOVE, proxy);
}

}
