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
#include "object/prototype_tree.hpp"
#include "object/object_proxy.hpp"

#include "object/object_exception.hpp"
#include "object/object_store.hpp"
#include "object/object_container.hpp"
#include "object/primary_key_analyzer.hpp"
#include <object/foreign_key_analyzer.hpp>

#include <iterator>
#include <algorithm>
#include <iostream>

namespace oos {

class relation_resolver : public generic_object_writer<relation_resolver>
{
public:
  typedef std::list<std::string> string_list_t;
  typedef string_list_t::const_iterator const_iterator;

public:
  relation_resolver(prototype_node &node)
    : generic_object_writer<relation_resolver>(this)
    , node_(node)
  {}
  virtual ~relation_resolver() {}

  void build(serializable *o) { o->serialize(*this); }
  template < class T >
  void write_value(const char*, const T&) {}

  void write_value(const char*, const char*, int) {}

  void write_value(const char *id, const object_container &x)
  {
    std::cout << "\nrelation resolver: resolving container " << id << " of type " << x.classname();

    prototype_iterator pi;
    object_base_producer *p = x.create_item_producer();
    if (p) {
//      pi = node_.tree->insert(p, id);
      pi = node_.tree->insert(p, p->classname());
      if (pi == node_.tree->end()) {
        throw object_exception("unknown prototype type");
      }
    } else {
      // insert new prototype
      // get prototype node of container item (child)
      pi = node_.tree->find(x.classname());
      if (pi == node_.tree->end()) {
        // if there is no such prototype node
        // insert a new one (it is automatically marked
        // as uninitialized)
        pi = prototype_iterator(new prototype_node());
        node_.tree->typeid_prototype_map_.insert(std::make_pair(x.classname(), prototype_tree::t_prototype_map()));
        node_.tree->prototype_map_[x.classname()] = pi.get();
      }
    }
    // add container node to item node
    // insert the relation
    pi->relations.insert(std::make_pair(node_.type, std::make_pair(&node_, id)));
  }

  void write_value(const char *id, const object_base_ptr &x)
  {
    std::cout << "\nrelation resolver: resolving serializable " << id << " of type " << x.type();
  }

private:
  prototype_node &node_;
};

prototype_iterator::prototype_iterator()
  : node_(0)
{}

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

prototype_iterator::~prototype_iterator()
{}

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

const_prototype_iterator::~const_prototype_iterator()
{}

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

prototype_tree::prototype_tree()
  : first_(new prototype_node)
  , last_(new prototype_node)
{
  // empty tree where first points to last and
  // last points to first sentinel
  first_->next = last_;
  last_->prev = first_;
}

prototype_tree::~prototype_tree()
{
  clear();
  delete last_;
  delete first_;
}

prototype_tree::iterator prototype_tree::insert(object_base_producer *producer, const char *type, bool abstract, const char *parent)
{
  // set node to root node
  prototype_node *parent_node = nullptr;
  if (parent != nullptr) {
    parent_node = find_prototype_node(parent);
    if (!parent_node) {
      throw object_exception("unknown prototype type");
    }
  }
  /*
   * try to insert new prototype node
   */
  prototype_node *node = acquire(producer, type, abstract);

  if (parent != nullptr) {
    parent_node->insert(node);
  } else {
    last_->prev->append(node);
  }

  return initialize(node);
}

prototype_tree::iterator prototype_tree::find(const char *type) {
  prototype_node *node = find_prototype_node(type);
  if (!node) {
    return end();
  }
  return prototype_iterator(node);
}

prototype_tree::const_iterator prototype_tree::find(const char *type) const {
  prototype_node *node = find_prototype_node(type);
  if (!node) {
    return end();
  }
  return const_prototype_iterator(node);
}

bool prototype_tree::empty() const {
  return first_->next == last_;
}


size_t prototype_tree::size() const {
  return (size_t) (std::distance(begin(), end()));
}

size_t prototype_tree::prototype_count() const
{
  return prototype_map_.size();
}

void prototype_tree::clear()
{
  while (first_->next != last_) {
    remove_prototype_node(first_->next, true);
  }
}

void prototype_tree::clear(const char *type)
{
  clear(find(type));
}

void prototype_tree::clear(const prototype_iterator &node)
{
  clear(node.get());
}

prototype_node* prototype_tree::clear(prototype_node *node)
{
  prototype_node *current = node->first->next;
  while (current != node->last.get()) {
    current = clear(current);
  }
  // finally link first to last and vice versa
  return remove_prototype_node(node, false);
}

int prototype_tree::depth(const prototype_node *node) const
{
  int d = 0;
  while (node->parent) {
    node = node->parent;
    ++d;
  }
  return d;
}

void prototype_tree::dump(std::ostream &out) const
{
  const_prototype_iterator node = begin();
  out << "digraph G {\n";
  out << "\tgraph [fontsize=10]\n";
  out << "\tnode [color=\"#0c0c0c\", fillcolor=\"#dd5555\", shape=record, style=\"rounded,filled\", fontname=\"Verdana-Bold\"]\n";
  out << "\tedge [color=\"#0c0c0c\"]\n";
  do {
    int d = depth(node.get());
    for (int i = 0; i < d; ++i) out << " ";
    out << "type: " << node->type << '\n';
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

prototype_tree::iterator prototype_tree::erase(const prototype_tree::iterator &i) {
  if (i == end() || i.get() == nullptr) {
    throw object_exception("invalid prototype iterator");
  }
  return remove_prototype_node(i.get(), i->depth == 0);
}

void prototype_tree::remove(const char *type) {
  prototype_node *node = find_prototype_node(type);
  if (!node) {
    throw object_exception("unknown prototype type");
  }
  remove_prototype_node(node, node->depth == 0);
}

void prototype_tree::remove(const prototype_iterator &node)
{
  remove_prototype_node(node.get(), false);
}

prototype_tree::iterator prototype_tree::begin()
{
  return prototype_iterator(first_->next);
}

prototype_tree::iterator prototype_tree::end()
{
  return prototype_iterator(last_);
}

prototype_tree::const_iterator prototype_tree::begin() const
{
  return prototype_tree::const_iterator(first_->next);
}

prototype_tree::const_iterator prototype_tree::end() const
{
  return prototype_tree::const_iterator(last_);
}

prototype_node* prototype_tree::find_prototype_node(const char *type) const {
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


prototype_node* prototype_tree::remove_prototype_node(prototype_node *node, bool is_root) {
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
  t_prototype_map::iterator j = prototype_map_.find(node->type.c_str());
  if (j != prototype_map_.end()) {
    prototype_map_.erase(j);
  }
  // find item in typeid map
  t_typeid_prototype_map::iterator k = typeid_prototype_map_.find(node->producer->classname());
  if (k != typeid_prototype_map_.end()) {
    k->second.erase(node->type);
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

/*
 * adjust the marker of all predeccessor nodes
 * self and last marker
 */
void prototype_tree::adjust_left_marker(prototype_node *root, object_proxy *old_proxy, object_proxy *new_proxy)
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

void prototype_tree::adjust_right_marker(prototype_node *root, object_proxy* old_proxy, object_proxy *new_proxy)
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

prototype_node *prototype_tree::acquire(object_base_producer *producer, const char *type, bool abstract)
{
  prototype_node *node = nullptr;
  t_prototype_map::iterator i = prototype_map_.find(type);
  if (i != prototype_map_.end()) {
    throw_object_exception("prototype already inserted: " << type);
  }

  /* unknown type name try for typeid
   * (unfinished prototype)
   */
  i = prototype_map_.find(producer->classname());
  if (i == prototype_map_.end()) {
    /*
     * no typeid found, seems to be
     * a new type
     * to be sure check in typeid map
     */
    t_typeid_prototype_map::iterator j = typeid_prototype_map_.find(producer->classname());
    if (j != typeid_prototype_map_.end() && j->second.find(type) != j->second.end()) {
      /* unexpected found the
       * typeid check for type
       */
      /* type found in typeid map
       * throw exception
       */
      throw object_exception("unexpectly found prototype");
    } else {
      /* insert new prototype and add to
       * typeid map
       */
      // create new one
      node = new prototype_node(this, producer, type, abstract);
    }
  } else {
    /* prototype is unfinished,
     * finish it, insert by type name,
     * remove typeid entry and add to
     * typeid map
     */
    node = i->second;
    node->initialize(this, producer, type, abstract);
    prototype_map_.erase(i);
  }

  return node;
}


prototype_tree::iterator prototype_tree::initialize(prototype_node *node)
{
  // store prototype in map
  // Todo: check return value
  prototype_map_.insert(std::make_pair(node->type, node)).first;
  typeid_prototype_map_[node->producer->classname()].insert(std::make_pair(node->type, node));

  // Analyze primary and foreign keys of node
  primary_key_analyzer pk_analyzer(*node);
  pk_analyzer.analyze();

  // Check if nodes serializable has 'to-many' relations
  std::unique_ptr<serializable> o(node->producer->create());
  relation_resolver rb(*node);
  rb.build(o.get());

  // Analyze primary and foreign keys of node
  foreign_key_analyzer fk_analyzer(*node);
  fk_analyzer.analyze();

  return prototype_iterator(node);
}

}
