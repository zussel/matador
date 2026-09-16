#ifndef MATADOR_SCHEMA_OBSERVER_HPP
#define MATADOR_SCHEMA_OBSERVER_HPP

#include "matador/query/internal/primary_key_generator_finder.hpp"
#include "matador/query/observer.hpp"
#include "matador/query/schema_node.hpp"
#include "matador/query/identity_pk_generator.hpp"
#include "matador/query/manual_pk_generator.hpp"
#include "matador/query/sequence_pk_generator.hpp"
#include "matador/query/table_pk_generator.hpp"

namespace matador::query {
class schema;
}
namespace matador::query::detail {
template <typename Type>
class schema_observer final : public observer<Type> {
public:
  explicit schema_observer(schema &s) : schema_(s) {}
  template <typename OtherType>
  explicit schema_observer(const schema_observer<OtherType> &x) : schema_(x.schema_) {}
  void on_attach(const schema_node &node, const Type &prototype) const override;
  void on_detach(const schema_node &node, const Type &prototype) const override;
  void on_insert(const Type &obj) override;
  void on_update(const Type &obj) override;
  void on_delete(const Type &obj) override;

private:
  template <class OtherType> friend class schema_observer;

  schema &schema_;
};

template <typename Type>
void schema_observer<Type>::on_attach(const schema_node &node, const Type &/*prototype*/) const {
  const std::type_index ti(typeid(Type));
  if (const auto &info = node.info<Type>().get(); info.has_primary_key()) {
    internal::primary_key_generator_finder finder;
    const auto generator_type = finder.find(info);
    // const auto it = schema_.insert_table(typeid(Type), node, generator_type);


    std::vector<column> columns;
    for (const auto &col: node.info().columns()) {
      columns.emplace_back(column::make_plain(nullptr, col.name(), "", col.type(), col.options()));
    }
    std::unique_ptr<abstract_pk_generator> pk_generator;
    switch (generator_type) {
    case generator_type::Identity:
      pk_generator = std::make_unique<identity_pk_generator>();
      break;
    case generator_type::Sequence:
      pk_generator = std::make_unique<sequence_pk_generator>(node.name() + "_pk_seq");
      break;
    case generator_type::Table:
      pk_generator = std::make_unique<table_pk_generator>("sequence_table", node.name());
      break;
    default:
      pk_generator = std::make_unique<manual_pk_generator>();
    }
    // return schema_nodes_.insert({ti, schema_node{table(node.name(), columns), std::move(pk_generator), node}}).first;


    // auto producer = std::make_unique<query_object_resolver_producer<Type>>(schema_, it->second.table(), it->second.node().info().primary_key_attribute()->name());
    // schema_.resolver_producers_[typeid(Type)] = std::move(producer);
  } else {
    // const auto it = schema_.insert_relation_table(typeid(Type), node);
    // auto producer = std::make_unique<query_object_resolver_producer<Type>>(schema_, it->second.table(), it->second.node().info().primary_key_attribute()->name());
    // schema_.resolver_producers_[typeid(Type)] = std::move(producer);
  }
}

template <typename Type>
void schema_observer<Type>::on_detach(const schema_node &/*node*/, const Type &/*prototype*/) const {}
template <typename Type>
void schema_observer<Type>::on_insert(const Type &/*obj*/) {}
template <typename Type>
void schema_observer<Type>::on_update(const Type &/*obj*/) {}
template <typename Type>
void schema_observer<Type>::on_delete(const Type &/*obj*/) {}

} // namespace matador::query::detail
#endif // MATADOR_SCHEMA_OBSERVER_HPP
