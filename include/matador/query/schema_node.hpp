#ifndef MATADOR_REPOSITORY_NODE_HPP
#define MATADOR_REPOSITORY_NODE_HPP

#include "matador/query/table_info.hpp"
#include "matador/query/internal/observer_list_creator.hpp"
#include "matador/query/internal/primary_key_generator_finder.hpp"
#include "matador/query/identity_pk_generator.hpp"
#include "matador/query/manual_pk_generator.hpp"
#include "matador/query/sequence_pk_generator.hpp"
#include "matador/query/table_pk_generator.hpp"
#include "matador/query/table_generator.hpp"

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
  [[nodiscard]] table_info<Type>& info() {
      return static_cast<table_info<Type>&>(*info_);
  }

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

  internal::primary_key_generator_finder finder;
  const Type obj;
  const auto generator_type = finder.find(obj);
  std::unique_ptr<abstract_pk_generator> pk_generator;
  switch (generator_type) {
  case generator_type::Identity:
    pk_generator = std::make_unique<identity_pk_generator>();
    break;
  case generator_type::Sequence:
    pk_generator = std::make_unique<sequence_pk_generator>(name + "_pk_seq");
    break;
  case generator_type::Table:
    pk_generator = std::make_unique<table_pk_generator>("sequence_table", name);
    break;
  default:
    pk_generator = std::make_unique<manual_pk_generator>();
  }

  node->info_ = std::make_unique<table_info<Type>>(
    *node,
    table_generator::generate<Type>(repo, name),
    std::move(pk_generator),
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

  node->info_ = std::make_unique<table_info<Type>>(
    *node,
    table_generator::generate(std::make_unique<Type>(join_column, inverse_join_column), repo, name, join_column, inverse_join_column),
    nullptr,
    std::move(observers),
    std::move(creator)
  );

  return node;
}
}
#endif //MATADOR_REPOSITORY_NODE_HPP
