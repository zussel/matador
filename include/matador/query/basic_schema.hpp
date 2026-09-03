#ifndef MATADOR_BASIC_SCHEMA_HPP
#define MATADOR_BASIC_SCHEMA_HPP

#include "matador/query/error_code.hpp"
#include "matador/query/schema_node.hpp"
#include "matador/query/schema_node_iterator.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

namespace matador::query {
error make_error(error_code ec, const std::string &msg);

class schema_node;

class basic_schema {
public:
  typedef const_schema_node_iterator const_iterator; /**< Shortcut for the list const iterator. */
  typedef schema_node_iterator iterator; /**< Shortcut for the list const iterator. */

  explicit basic_schema(std::string name = "");
  ~basic_schema();

  /**
   *
   * @param node Node to attach
   * @param parent Name of parent node
   * @return Attached node
   */
  [[nodiscard]] result<schema_node*, error> attach_node(std::unique_ptr<schema_node> &&node, const std::string &parent);

  /**
   * Detaches a given node from the schema. If the
   * node is a parent of other nodes, these nodes  are
   * detached as well.
   *
   * @param node Node to detach from schema
   * @return Result object indicating success or failure
   */
  [[nodiscard]] result<void, error> detach(const schema_node *node);

  /**
   * Return the first schema node.
   *
   * @return The first schema node iterator.
   */
  [[nodiscard]] const_iterator begin() const;

  /**
   * Return the last schema node.
   *
   * @return The last schema node iterator.
   */
  [[nodiscard]] const_iterator end() const;

  /**
   * Returns true if the schema contains
   * no schema nodes.
   *
   * @return True if the schema is empty
   */
  [[nodiscard]] bool empty() const;

  /**
   * Returns the current number of the schema node.
   *
   * @return Number of schema nodes
   */
  [[nodiscard]] size_t size() const;

  /**
   * Returns the name of the schema.
   *
   * @return The name of the schema
   */
  [[nodiscard]] std::string name() const;

  [[nodiscard]] bool contains(const std::string &name) const;
  [[nodiscard]] bool contains(const std::type_index &index) const;
  template < typename Type >
  [[nodiscard]] bool contains() const {
    return contains(std::type_index(typeid(Type)));
  }


  [[nodiscard]] result<basic_table_info_ref, error> basic_info(const std::type_index& ti) const;
  [[nodiscard]] result<basic_table_info_ref, error> basic_info(const std::string &name) const;
  template<typename Type>
  [[nodiscard]] result<basic_table_info_ref, error> basic_info() const {
    return basic_info(std::type_index(typeid(Type)));
  }

  [[nodiscard]] result<const column*, error> primary_key_attribute(const std::type_index &ti) const;

  void dump(std::ostream &os) const;
  static void dump(std::ostream &os, const schema_node& node);

protected:
  using t_node_map = std::unordered_map<std::string, schema_node*>;
  using t_type_index_node_map = std::unordered_map<std::type_index, schema_node*>;

protected:
  [[nodiscard]] const_iterator find_node(const std::string &name) const;
  [[nodiscard]] const_iterator find_node(const std::type_index &type_index) const;
  template<typename Type>
  [[nodiscard]] const_iterator find_node() const {
    return find_node(std::type_index(typeid(Type)));
  }
  [[nodiscard]] iterator find_node(const schema_node *node) const;

  [[nodiscard]] t_node_map::iterator find_node_by_name(const std::string &name);

  [[nodiscard]] bool has_node(const std::string &name) const;
  [[nodiscard]] bool has_node(const std::type_index &index) const;
  [[nodiscard]] bool has_node(const schema_node *node) const;

  static void insert_node(schema_node *parent, schema_node *child);
  void remove_node(schema_node *node);

  [[nodiscard]] bool expecting_relation_node(const std::string &name) const;
  void expect_relation_node(const std::string &name, const std::type_index &ti);
  void remove_expected_relation_node(const std::string &name);

  [[nodiscard]] std::shared_ptr<table> provide_table_in_advance(const std::type_index &ti, const std::shared_ptr<table>& obj);
  [[nodiscard]] bool has_table_for_type(const std::type_index &ti) const;
  [[nodiscard]] std::shared_ptr<table> table_for_type(const std::type_index &ti) const;
  void remove_table_for_type(const std::type_index &ti);

  [[nodiscard]] bool is_node_announced(const std::type_index &ti) const;
  void push_announce_node(const std::type_index &ti, std::unique_ptr<schema_node> &&node);
  [[nodiscard]] schema_node* announce_node(const std::type_index &ti) const;
  [[nodiscard]] std::unique_ptr<schema_node> pop_announce_node(const std::type_index &ti);

protected:
  friend class table_generator;
  template < typename NodeType, template<typename> typename ...Observers >
  friend class relation_completer;

  std::string name_;
  std::unique_ptr<schema_node> root_;
  std::vector<std::unique_ptr<schema_node>> nodes_;

  t_node_map nodes_by_name_;
  t_type_index_node_map nodes_by_type_;
  // logger::logger log_;

  std::unordered_map<std::type_index, std::unique_ptr<schema_node>> announced_node_;
  std::unordered_map<std::type_index, column*> missing_references_{};
  std::unordered_map<std::string, std::type_index> expected_relation_nodes_;
  std::unordered_map<std::type_index, std::shared_ptr<table>> table_by_type_{};
};
}
#endif //MATADOR_BASIC_SCHEMA_HPP
