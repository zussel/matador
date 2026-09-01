#ifndef MATADOR_REPOSITORY_NODE_HPP
#define MATADOR_REPOSITORY_NODE_HPP

#include "matador/query/table_info.hpp"
#include "matador/query/internal/observer_list_creator.hpp"

#include <memory>

namespace matador::query {

class basic_table_info;
class basic_schema;

class schema_node final {
public:
  using node_ptr = schema_node*;
  template< typename Type>
  using creator_func = std::function<std::unique_ptr<Type>()>;

  template < typename Type, template<typename> typename... Observers >
  static std::unique_ptr<schema_node> make_node(basic_schema& repo,
                                                    const std::string& name,
                                                    creator_func<Type> creator,
                                                    std::vector<std::unique_ptr<observer<Type>>>&& observers);
  template < typename Type, template<typename> typename... Observers >
  static std::unique_ptr<schema_node> make_relation_node(basic_schema& repo,
                                                             const std::string& name,
                                                             const std::string& join_column,
                                                             const std::string& inverse_join_column,
                                                             creator_func<Type> creator,
                                                             std::vector<std::unique_ptr<observer<Type>>>&& observers);

  explicit schema_node(basic_schema& repo);
  schema_node(const schema_node& other) = delete;
  schema_node(schema_node&& other) = delete;
  schema_node& operator=(const schema_node& other) = delete;
  schema_node& operator=(schema_node&& other) = delete;
  ~schema_node() = default;

  [[nodiscard]] std::string name() const;
  [[nodiscard]] std::type_index type_index() const;

  [[nodiscard]] node_ptr next() const;
  [[nodiscard]] node_ptr prev() const;

  [[nodiscard]] const basic_table_info& info() const;

  void update_name(const std::string& name);

  template <typename Type>
  [[nodiscard]] object_info_ref<Type> info() const {
      return std::ref(static_cast<const table_info<Type>&>(*info_));
  }

  [[nodiscard]] const basic_schema& schema() const;

  [[nodiscard]] bool has_children() const;

  void on_attach() const;
  void on_detach() const;

private:
  schema_node(basic_schema& repo, const std::type_index& ti);
  schema_node(basic_schema& repo, std::string name, const std::type_index& ti);

  void unlink();

private:
  friend class basic_schema;
  template<typename Type, template<typename> typename... Observers>
  friend class relation_completer;
  template < typename NodeType, template<typename> typename ...Observers >
  friend class foreign_node_completer;
  friend class const_repository_node_iterator;

  basic_schema &repo_;
  std::type_index type_index_;
  std::unique_ptr<basic_table_info> info_;

  schema_node* parent_{nullptr};
  schema_node* previous_sibling_{nullptr};
  schema_node* next_sibling_{nullptr};
  std::unique_ptr<schema_node> first_child_;
  std::unique_ptr<schema_node> last_child_;

  std::string name_;
  size_t depth_{0};
};

template<typename Type, template <typename> class ... Observers>
std::unique_ptr<schema_node> schema_node::make_node(basic_schema &repo,
                                                    const std::string &name,
                                                    creator_func<Type> creator,
                                                    std::vector<std::unique_ptr<observer<Type>>> &&observers) {
  const std::type_index ti(typeid(Type));
  auto node = std::unique_ptr<schema_node>(new schema_node(repo, name, ti));

  internal::observer_list_creator<Type, Observers...>::create_missing(observers);

  // auto obj = object_generator::generate<Type>(creator(), repo, name);
  node->info_ = std::make_unique<table_info<Type>>(
    *node,
    std::make_shared<table>(),
    std::move(observers),
    std::move(creator)
  );

  return node;
}

template<typename Type, template <typename> class ... Observers>
std::unique_ptr<schema_node> schema_node::make_relation_node(basic_schema &repo,
                                                             const std::string &name,
                                                             const std::string &join_column,
                                                             const std::string &inverse_join_column,
                                                             creator_func<Type> creator,
                                                             std::vector<std::unique_ptr<observer<Type>>> &&observers) {
  const std::type_index ti(typeid(Type));
  auto node = std::unique_ptr<schema_node>(new schema_node(repo, name, ti));

  internal::observer_list_creator<Type, Observers...>::create_missing(observers);

  // auto obj = object_generator::generate<Type>(creator(), repo, name, join_column, inverse_join_column);
  node->info_ = std::make_unique<table_info<Type>>(
    *node,
    std::make_shared<table>(),
    std::move(observers),
    std::move(creator)
  );

  return node;
}
}
#endif //MATADOR_REPOSITORY_NODE_HPP
