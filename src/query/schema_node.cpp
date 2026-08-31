#include "matador/query/schema_node.hpp"

#include "matador/query/basic_schema.hpp"

#include <memory>
#include <utility>

namespace matador::query {
schema_node::schema_node(basic_schema &repo)
: repo_(repo)
, type_index_(typeid(detail::null_type)){
}

schema_node::schema_node(basic_schema &repo, const std::type_index& ti)
: repo_(repo)
, type_index_(ti) {
}

schema_node::schema_node(basic_schema &repo, std::string name, const std::type_index& ti)
: repo_(repo)
, type_index_(ti)
, first_child_(std::make_unique<schema_node>(repo))
, last_child_(std::make_unique<schema_node>(repo))
, name_(std::move(name)) {
  first_child_->next_sibling_ = last_child_.get();
  last_child_->previous_sibling_ = first_child_.get();
}

std::string schema_node::name() const {
  return name_;
}

std::type_index schema_node::type_index() const {
  return type_index_;
}

const basic_table_info &schema_node::info() const {
  return *info_;
}

void schema_node::update_name(const std::string& name) {
  name_ = name;
  info_->update_name(name);
}

const basic_schema& schema_node::schema() const {
  return repo_;
}

bool schema_node::has_children() const {
  return first_child_->next_sibling_ != last_child_.get();
}
void schema_node::on_attach() const {
  info_->on_attach();
}

void schema_node::on_detach() const {
  info_->on_detach();
}

schema_node::node_ptr schema_node::next() const {
  // if we have a child, child is the next iterator to return
  // (if we don't iterate over the siblings)
  if (first_child_ && first_child_->next_sibling_ != last_child_.get()) {
    return first_child_->next_sibling_;
  }
  // if there is no child, we check for sibling
  // if there is a sibling, this is our next iterator to return
  // if not, we go back to the parent
  auto *node = this;
  while (node->parent_ && node->next_sibling_ == node->parent_->last_child_.get()) {
    node = node->parent_;
  }
  return node->parent_ ? node->next_sibling_ : node->last_child_.get();
}

schema_node::node_ptr schema_node::prev() const {
  // if the node has a previous sibling, we set it
  // as our next iterator. then we check if there
  // are last children. if so, we set the last-last
  // child as our iterator
  if (previous_sibling_ && previous_sibling_->previous_sibling_) {
    auto node = previous_sibling_;
    while (node->last_child_ && node->first_child_->next_sibling_ != node->last_child_.get()) {
      node = node->last_child_->previous_sibling_;
    }
    return node;
  }
  // if there is no previous sibling, our next iterator
  // is the parent of the node
  return parent_->parent_ ? parent_ : parent_->first_child_->next_sibling_;
}

void schema_node::unlink() {
  previous_sibling_->next_sibling_ = next_sibling_;
  next_sibling_->previous_sibling_ = previous_sibling_;
  next_sibling_ = nullptr;
  previous_sibling_ = nullptr;
}
}
