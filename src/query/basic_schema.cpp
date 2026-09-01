#include "matador/query/basic_schema.hpp"

#include "matador/query/error_code.hpp"
#include "matador/query/schema_node.hpp"
#include "matador/query/schema_node_iterator.hpp"

// #include "matador/logger/log_manager.hpp"

#include <algorithm>

namespace matador::query {
utils::error make_error(const error_code ec, const std::string &msg) {
  return utils::error(ec, msg);
}

basic_schema::basic_schema(std::string name)
: name_(std::move(name))
, root_(std::make_unique<schema_node>(*this))
/*, log_(logger::create_logger("schema"))*/ {
  root_->first_child_ = std::make_unique<schema_node>(*this);
  root_->last_child_ = std::make_unique<schema_node>(*this);
  root_->first_child_->next_sibling_ = root_->last_child_.get();
  root_->last_child_->previous_sibling_ = root_->first_child_.get();
  root_->info_ = std::make_unique<null_info>(*root_);
}

basic_schema::~basic_schema() {
  while (root_ && root_->first_child_->next_sibling_ != root_->last_child_.get()) {
    remove_node(root_->first_child_->next_sibling_);
  }
}

utils::result<void, utils::error> basic_schema::detach(const schema_node *node) {
  if (!node) {
    return utils::failure(make_error(error_code::InvalidArgument, "Node is null."));
  }

  auto it = find_node(node);
  if (it == iterator(root_->last_child_.get())) {
    return utils::failure(make_error(error_code::NodeNotFound, "Node is not attached to this repository."));
  }

  // log_.debug("detach node '%s' (type: %s)", node->name().c_str(), node->type_index().name());

  remove_node(it.get());

  return utils::ok<void>();
}

basic_schema::const_iterator basic_schema::begin() const {
  return const_iterator(root_->first_child_->next_sibling_);
}

basic_schema::const_iterator basic_schema::end() const {
  return const_iterator(root_->last_child_.get());
}

bool basic_schema::empty() const {
  return root_->first_child_.get() == root_->last_child_->previous_sibling_;
}

size_t basic_schema::size() const {
  return static_cast<size_t>(std::distance(begin(), end()));
}

std::string basic_schema::name() const {
  return name_;
}

bool basic_schema::contains( const std::string& name ) const {
  return nodes_by_name_.count(name) > 0;
}

bool basic_schema::contains( const std::type_index& index ) const {
  return nodes_by_type_.count(index) > 0;
}

utils::result<basic_table_info_ref, utils::error> basic_schema::basic_info(const std::type_index &ti) const {
  const auto it = find_node(ti);
  if (it == end()) {
    return utils::failure(make_error(error_code::NodeNotFound, "Node '" + std::string(ti.name()) + "' not found."));
  }

  return utils::ok(basic_table_info_ref{it->info()});
}

utils::result<basic_table_info_ref, utils::error> basic_schema::basic_info(const std::string& name) const {
    const auto it = find_node(name);
    if (it == end()) {
      return utils::failure(make_error(error_code::NodeNotFound, "Node '" + name + "' not found."));
    }

    return utils::ok(basic_table_info_ref{it->info()});
}

utils::result<const column*, utils::error> basic_schema::primary_key_attribute(const std::type_index &ti) const {
  const auto it = find_node(ti);
  if (it == end()) {
    return utils::failure(make_error(error_code::NodeNotFound, "Node '" + std::string(ti.name()) + "' not found."));
  }

  if (!it->info().has_primary_key()) {
    return utils::failure(make_error(error_code::NoPrimaryKey, "Object '" + it->name() + "' does not have a primary key."));
  }
  return utils::ok(it->info().primary_key_attribute());
}

void basic_schema::dump(std::ostream &os) const {
  os << name_ << std::endl;
  for (const auto &node : *this) {
    dump(os, node);
  }
  os << "\n";
}

void basic_schema::dump( std::ostream& os, const schema_node& node ) {
    os << "node [" << node.name() << "]\n";
    for (auto it = node.info().endpoint_begin(); it != node.info().endpoint_end(); ++it) {
        os << "  " << node.name() << "::" << it->second->field_name() << " (" << it->second->type_name() << ")";
        if (it->second->foreign_endpoint()) {
            os << " <---> " << it->second->node().name() << "::" << it->second->foreign_endpoint()->field_name() << " (" << it->second->foreign_endpoint()->type_name() << ")\n";
        } else {
            os << " -> " << it->second->node().name() << " (type: " << it->second->node().name() << ")\n";
        }
    }
  os << "\n";
}

utils::result<schema_node*, utils::error> basic_schema::attach_node(std::unique_ptr<schema_node> &&node, const std::string &parent) {
  if (!node) {
    return utils::failure(make_error(error_code::InvalidArgument, "Node is null."));
  }

  if (has_node(node.get())) {
    return utils::failure(make_error(error_code::NodeAlreadyExists, "Node '" + node->name() + "' already exists."));
  }

  auto parent_node = root_.get();
  if (!parent.empty()) {
    const auto it = find_node_by_name(parent);
    if (it == nodes_by_name_.end()) {
      return utils::failure(make_error(error_code::NodeNotFound, "Parent node '" + parent + "' not found."));
    }
    parent_node = it->second;
  }

  if (nodes_by_name_.count(node->name()) > 0) {
    return utils::failure(make_error(error_code::NodeAlreadyExists, "Node '" + node->name() + "' already exists."));
  }

  if (nodes_by_type_.count(node->type_index()) > 0) {
    return utils::failure(make_error(error_code::NodeAlreadyExists, "Node type '" + std::string(node->type_index().name()) + "' already exists."));
  }

  auto *raw_node = node.get();

  // log_.info("attach: insert node '%s' (type: %s)", raw_node->name().c_str(), raw_node->type_index().name());

  insert_node(parent_node, raw_node);
  nodes_by_name_.emplace(raw_node->name(), raw_node);
  nodes_by_type_.emplace(raw_node->type_index(), raw_node);
  nodes_.push_back(std::move(node));

  raw_node->on_attach();

  return utils::ok(raw_node);
}

basic_schema::const_iterator basic_schema::find_node(const std::string &name) const {
  const auto i = nodes_by_name_.find(name);
  if (i == nodes_by_name_.end()) {
    return end();
  }
  return const_iterator(i->second);
}

basic_schema::const_iterator basic_schema::find_node(const std::type_index &type_index) const {
  const auto i = nodes_by_type_.find(type_index);
  if (i == nodes_by_type_.end()) {
    return end();
  }
  return const_iterator(i->second);
}

basic_schema::iterator basic_schema::find_node(const schema_node *node) const {
  if (!node) {
    return iterator(root_->last_child_.get());
  }

  if (const auto i = nodes_by_name_.find(node->name()); i != nodes_by_name_.end()) {
    return iterator(i->second);
  }

  const auto i = nodes_by_type_.find(node->type_index());
  if (i == nodes_by_type_.end()) {
    return iterator(root_->last_child_.get());
  }

  return iterator(i->second);
}

void basic_schema::insert_node(schema_node *parent, schema_node *child) {
  child->parent_ = parent;
  child->previous_sibling_ = parent->last_child_->previous_sibling_;
  child->next_sibling_ = parent->last_child_.get();
  /*
   *      +-----------------------------<- (first) parent (last) -> ----------------------------+
   *      |                                                                                     |
   *    first (next) ->   <- (prev) child_1 (next) ->  <- (prev) new_child (next) -> <- (prev) last
   *                                                      ^^^^^^^ inserted ^^^^^^
   */
  if (const auto prev_sib = parent->last_child_->previous_sibling_) {
    prev_sib->next_sibling_ = child;
  }
  parent->last_child_->previous_sibling_ = child;
  // set depth
  // child->depth = depth + 1;
}

void basic_schema::remove_node(schema_node *node) {
  if (!node) {
    return;
  }

  while (node->has_children()) {
    remove_node(node->last_child_->previous_sibling_);
  }

  node->on_detach();
  node->unlink();

  nodes_by_name_.erase(node->name());
  nodes_by_type_.erase(node->type_index());

  const auto it = std::find_if(nodes_.begin(), nodes_.end(), [node](const auto &owned_node) {
    return owned_node.get() == node;
  });

  if (it != nodes_.end()) {
    nodes_.erase(it);
  }
}

std::unordered_map<std::string, schema_node *>::iterator basic_schema::find_node_by_name(const std::string &name) {
  return nodes_by_name_.find(name);
}

bool basic_schema::has_node(const std::string &name) const {
  return nodes_by_name_.count(name) > 0;
}

bool basic_schema::has_node(const std::type_index &index) const {
  return nodes_by_type_.count(index) > 0;
}

bool basic_schema::has_node(const schema_node *node) const {
  return node && (nodes_by_name_.count(node->name()) > 0 || nodes_by_type_.count(node->type_index()) > 0);
}

bool basic_schema::expecting_relation_node( const std::string& name ) const {
  return expected_relation_nodes_.count(name) > 0;
}

void basic_schema::expect_relation_node(const std::string &name, const std::type_index &ti) {
  expected_relation_nodes_.insert({name, ti});
}

void basic_schema::remove_expected_relation_node( const std::string& name ) {
  expected_relation_nodes_.erase(name);
}

std::shared_ptr<table> basic_schema::provide_table_in_advance(const std::type_index &ti, const std::shared_ptr<table>& obj) {
  return table_by_type_.insert({ti, obj}).first->second;
}

bool basic_schema::has_table_for_type(const std::type_index &ti) const {
  return table_by_type_.count(ti) > 0;
}

std::shared_ptr<table> basic_schema::table_for_type(const std::type_index &ti) const {
  return table_by_type_.at(ti);
}

void basic_schema::remove_table_for_type(const std::type_index &ti) {
  table_by_type_.erase(ti);
}

bool basic_schema::is_node_announced(const std::type_index &ti) const {
  return announced_node_.count(ti) > 0;
}

void basic_schema::push_announce_node(const std::type_index &ti, std::unique_ptr<schema_node> &&node) {
  announced_node_.insert({ti, std::move(node)});
}

schema_node* basic_schema::announce_node(const std::type_index &ti) const {
  return announced_node_.find(ti)->second.get();
}

std::unique_ptr<schema_node> basic_schema::pop_announce_node(const std::type_index &ti) {
  const auto it = announced_node_.find(ti);
  if (it == announced_node_.end()) {
    return nullptr;
  }
  auto node = std::move(it->second);
  announced_node_.erase(it);
  return node;
}
}