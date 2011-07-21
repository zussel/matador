#include "object/object_store.hpp"
#include "object/object_atomizer.hpp"
#include "object/object_list.hpp"

#include <tr1/functional>
#include <memory>
#include <iostream>
#include <typeinfo>
#include <algorithm>

using std::tr1::placeholders::_1;

class equal_type : public std::unary_function<const object_store::prototype_node*, bool> {
public:
  explicit equal_type(const std::string &type) : type_(type) {}

  bool operator() (const object_store::prototype_node *x) const {
    return x->type == type_;
  }
private:
  const std::string &type_;
};

class object_creator : public object_atomizer
{
public:
  object_creator(object_store &ostore) : ostore_(ostore) {}
  virtual ~object_creator() {}

  virtual void read_object(const char* id, base_object_ptr &x)
  {
    if (!x.is_reference()) {
      // create object
    }
  }
  virtual void read_object_list(const char* id, object_list_base &x)
  {
    ostore_.insert_object_list(x);
  }
private:
  object_store &ostore_;
};

object_store::prototype_node::prototype_node()
  : parent(NULL)
  , prev(NULL)
  , next(NULL)
  , first(NULL)
  , last(NULL)
  , producer(NULL)
  , depth(0)
  , count(0)
{
}

object_store::prototype_node::prototype_node(object_base_producer *p, const char *t)
  : parent(NULL)
  , prev(NULL)
  , next(NULL)
  , first(new prototype_node)
  , last(new prototype_node)
  , producer(p)
  , depth(0)
  , count(0)
  , type(t)
{
  first->next = last;
  last->prev = first;
}

object_store::prototype_node::~prototype_node()
{
  delete producer;
}

void
object_store::prototype_node::clear()
{
}

bool
object_store::prototype_node::empty() const
{
  return op_first->next == op_last;
}

unsigned long
object_store::prototype_node::size() const
{
  return count;
}

void
object_store::prototype_node::insert(prototype_node *child)
{
  child->parent = this;
  child->prev = last->prev;
  child->next = last;
  last->prev->next = child;
  last->prev = child;
  // set depth
  child->depth = depth + 1;
  // set object proxy pointer
  // 1. first
  if (op_first->next == op_last) {
    // node hasn't any object (proxy)
    child->op_first = op_first;
  } else {
    // node has some objects (proxy)
    child->op_first = op_last->prev;
  }
  // 2. marker
  child->op_marker = op_last;
  // 3. last
  child->op_last = op_last;
}    

object_store::prototype_node* object_store::prototype_node::next_node() const
{
  // if we have a child, child is the next iterator to return
  // (if we don't do iterate over the siblings)
  if (first->next != last)
    return first->next;
  else {
    // if there is no child, we check for sibling
    // if there is a sibling, this is our next iterator to return
    // if not, we go back to the parent
    const prototype_node *node = this;
    while (node->parent && node->next == node->parent->last) {
      node = node->parent;
    }
    return node->next;
  }
}

object_store::prototype_node* object_store::prototype_node::previous_node() const
{
  // if node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last childs. if so, we set the last last
  // child as our iterator
  if (prev && prev->prev) {
    const prototype_node *node = prev;
    while (node->last && node->first->next != node->last) {
      node = node->last->prev;
    }
    return const_cast<prototype_node*>(node);
    // if there is no previous sibling, our next iterator
    // is the parent of the node
  } else {
    return parent;
  }
}

std::ostream& operator <<(std::ostream &os, const object_store::prototype_node &pn)
{
  os << "node [" << &pn << "] depth [" << pn.depth << "] type [" << pn.type << "] class [" << pn.producer->classname() << "]";
  if (pn.op_first->obj) {
    os << " first [" << *pn.op_first->obj << "]";
  } else {
    os << " first [op: " << pn.op_first << "]";
  }
  if (pn.op_marker->obj) {
    os << " marker [" << *pn.op_marker->obj << "]";
  } else {
    os << " marker [op: " << pn.op_marker << "]";
  }
  if (pn.op_last->obj) {
    os << " last [" << *pn.op_last->obj << "]";
  } else {
    os << " last [op: " << pn.op_last << "]";
  }
  int i = 0;
  object_proxy *iop = pn.op_first;
  while (iop->next != pn.op_marker) {
    ++i;
    iop = iop->next;
  }
  os << " (# " << i << ")";
  return os;
}

object_store::object_store()
  : root_(new prototype_node(new object_producer<object>, "OBJECT"))
  , id_(0)
  , first_(new object_proxy(NULL, this))
  , last_(new object_proxy(NULL, this))
{
  // set marker for root element
  root_->op_first = first_;
  root_->op_marker = last_;
  root_->op_last = last_;
  root_->op_first->next = root_->op_last;
  root_->op_last->prev = root_->op_first;
}

object_store::~object_store()
{
  delete root_;
}

bool
object_store::insert_prototype(object_base_producer *producer, const char *type, const char *parent)
{
  // set node to root node
  prototype_node *node = root_;
  if (parent) {
    // parent type name is set search parent node
    node = find_prototype(node, equal_type(parent));
    if (!node) {
      // couldn't find parent, raise exception
      //throw new object_exception("couldn't find parent prototype");
      return false;
    }
  }
  // find prototype node 'type' starting from node
  prototype_node *n = find_prototype(node, equal_type(type));
  if (n) {
    // prototype already exists, raise exception
    //throw new object_exception("prototype already exists");
    return false;
  } else {
    // create new prototype node
    n = new prototype_node(producer, type);
    // append as child to parent prototype node
    node->insert(n);
    // store prototype in map
    prototype_node_map_.insert(std::make_pair(type, n));
    // return success
    return true;
  }
}

bool object_store::remove_prototype(const char *type)
{
  return true;
}

int depth(object_store::prototype_node *node)
{
  int d = 0;
  while (node->parent) {
    node = node->parent;
    ++d;
  }
  return d;
}

void object_store::dump_prototypes(std::ostream &out) const
{
  prototype_node *node = root_;
  out << "dumping prototype tree:\n";
  do {
    int d = depth(node);
    for (int i = 0; i < d; ++i) out << " ";
    out << *node << "\n";
    node = node->next_node();
  } while (node);
  out << std::endl;
}

void object_store::dump_objects(std::ostream &out) const
{
  out << "dumping all objects\n";

  object_proxy *i = first_;
  while (i->next != last_) {
    i = i->next;
    out << *i->obj << " (prev [" << i->prev->obj << "] next [" << i->next->obj << "])\n";
  }
}

object* object_store::create(const char *type) const
{
  prototype_node *node = find_prototype(root_, equal_type(type));
  if (!node) {
    return NULL;
  }
  return node->producer->create();
}

void object_store::mark_modified(object_proxy *oproxy)
{
  std::for_each(observer_list_.begin(), observer_list_.end(), std::tr1::bind(&object_observer::on_update, _1, oproxy->obj));
}

void object_store::register_observer(object_observer *observer)
{
  if (std::find(observer_list_.begin(), observer_list_.end(), observer) == observer_list_.end()) {
    observer_list_.push_back(observer);
  }
}

void object_store::unregister_observer(object_observer *observer)
{
  t_observer_list::iterator i = std::find(observer_list_.begin(), observer_list_.end(), observer);
  if (i != observer_list_.end()) {
    delete *i;
    observer_list_.erase(i);
  }
}

void object_store::cleanup()
{
  // remove all object_proxy from object_proxy
  // link list
  // set object link to null
  // erase object_proxy from to delete list
}

object* object_store::insert_object(object *o)
{
  // find type in tree
  if (!o) {
    // throw exception
    return NULL;
  }
  // find prototype node of object
  prototype_node *node = find_prototype(root_, equal_classname(typeid(*o).name()));
  if (!node) {
    // raise exception
    //throw new object_exception("couldn't insert element of type [" + o->type() + "]\n");
    return NULL;
  }
  // retrieve and set new unique number into object
  //object->id(UniqueProducer::instance().number("default"));
  o->id(++id_);

  // insert new element node
  object_proxy *oproxy = new object_proxy(o, this);
  
  // check amount of object in subtree
  if (node->count >= 2) {
    // there are more than two objects (normal case)
    // insert before last last
    //std::cout << "more than two elements: inserting " << *o << " before second last (" << *node->op_marker->prev->obj << ")\n";
    node->op_marker->prev->insert(oproxy);
  } else if (node->count == 1) {
    // there is one object in subtree
    // insert as first; adjust "left" marker
    /*if (node->op_marker->prev->obj) {
      std::cout << "one element in list: inserting " << *o << " as first (before: " << *node->op_marker->prev->obj << ")\n";
    } else {
      std::cout << "one element in list: inserting " << *o << " as first (before: [0])\n";
    }*/
    node->op_marker->prev->insert(oproxy);
    adjust_left_marker(node, oproxy);
  } else /* if (node->count == 0) */ {
    // there is no object in subtree
    // insert as last; adjust "right" marker
    /*if (node->op_marker->obj) {
      std::cout << "list is empty: inserting " << *o << " as last before " << *node->op_marker->obj << "\n";
    } else {
      std::cout << "list is empty: inserting " << *o << " as last before [0]\n";
    }*/
    node->op_marker->insert(oproxy);
    adjust_left_marker(node, oproxy);
    adjust_right_marker(node, oproxy->prev, oproxy);
  }
  // create object
  object_creator oc(*this);
  o->read_from(&oc);
  // notify observer
  std::for_each(observer_list_.begin(), observer_list_.end(), std::tr1::bind(&object_observer::on_insert, _1, o));
  // dump objects
  //std::cout << std::endl;
/*  dump_objects(std::cout);
  std::cout << std::endl;*/
  // adjust size
  ++node->count;
  // insert element into hash map
  //  objectmap_[object->id()] = object;
  // set this into persistent object
  o->proxy_ = oproxy;
  return o;
}

bool object_store::remove_object(object *o)
{
  // find prototype node
  prototype_node *node = find_prototype(root_, equal_classname(typeid(*o).name()));
  if (!node) {
    // raise exception
    //throw new object_exception("couldn't insert element of type [" + o->type() + "]\n");
    return false;
  }

  if (o->proxy_ == node->op_first->next) {
    // adjust left marker
    adjust_left_marker(node, node->op_first->next->next);
  }
  if (o->proxy_ == node->op_marker->prev) {
    // adjust right marker
    adjust_right_marker(node, o->proxy_, node->op_marker->prev->prev);
  }
  // unlink object_proxy
  o->proxy_->remove();

  // notify observer
  std::for_each(observer_list_.begin(), observer_list_.end(), std::tr1::bind(&object_observer::on_delete, _1, o));
  // if object was last object in list of prototype node
  // adjust prototype node last
  // if object was last object (now empty)
  // adjust prototype node first, marker and last

  // mark object proxy as deleted
  // set object in object_proxy to null
  o->proxy_->obj = NULL;
  // return true
  return true;
}

bool object_store::insert_object_list(object_list_base &olb)
{
  olb.ostore_ = this;
  insert_object(olb.first_obj_);
  insert_object(olb.last_obj_);
  return true;
}

bool is_child_of(object_store::prototype_node *start, object_store::prototype_node *node)
{
  while (start->depth < node->depth) {
    node = node->parent;
  }
  return node == start;
}

void object_store::adjust_left_marker(object_store::prototype_node *node, object_proxy *oproxy)
{
  // store start node
  object_store::prototype_node *start = node;
  // get previous node
  node = node->previous_node();
  while (node) {
    bool do_break = node->op_first->next != oproxy;
    /*
    if (node->op_first->next == oproxy) {
      std::cout << "before adjusting left node: " << *node << "\n";
      node->op_last = oproxy;
      node->op_marker = oproxy;
      std::cout << "after adjusting left node: " << *node << "\n";
    } else {
    */
      //std::cout << "break: before adjusting left node: " << *node << "\n";
      node->op_marker = oproxy;
      //if (start->parent != node || node->empty()) {
      if (!is_child_of(node, start) || node->empty()) {
        node->op_last = oproxy;
      }
      //std::cout << "break: after adjusting left node: " << *node << "\n";
      /*break;
    }*/
      if (do_break) {
        break;
      }
    node = node->previous_node();
  }
}

void object_store::adjust_right_marker(object_store::prototype_node *node, object_proxy *old_proxy, object_proxy *new_proxy)
{
  // store start node
  object_store::prototype_node *start = node;
  // get previous node
  node = node->next_node();
  bool first = true;
  while (node) {
    if (node->op_first == old_proxy) {
      //std::cout << "before adjusting right node: " << *node << "\n";
      node->op_first = new_proxy;
      //std::cout << "after adjusting right node: " << *node << "\n";
    } else {
      //std::cout << "break: before adjusting right node: " << *node << "\n";
      node->op_first = new_proxy;
      //std::cout << "break: after adjusting right node: " << *node << "\n";
      break;
    }
    // check watermark
    if (first) {
      if (start->depth == node->depth && !node->empty()) {
        break;
      }
      first = false;
    } else if (start->depth <= node->depth && !node->empty()) {
      break;
    }
    node = node->next_node();
  }
}
