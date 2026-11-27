#ifndef MATADOR_PRODUCER_CREATOR_HPP
#define MATADOR_PRODUCER_CREATOR_HPP

#include "matador/query/builder/query.hpp"
#include "matador/query/criteria/criteria_operators.hpp"
#include "matador/query/internal/select_query_builder.hpp"
#include "matador/query/schema/basic_schema.hpp"

#include "matador/query/resolver/query_object_resolver.hpp"
#include "matador/query/resolver/resolver_producer.hpp"
#include "matador/query/resolver/query_collection_resolver.hpp"
#include "matador/query/resolver/joined_collection_resolver_producer.hpp"

namespace matador::query {
class dialect;
class statement;

class resolver_producer_registry final {
public:
  using joined_object_producer_map =
      std::unordered_map<collection_composite_key, std::unique_ptr<abstract_object_resolver_producer>,
                         collection_composite_key_hash>;
  using collection_producer_map =
      std::unordered_map<collection_composite_key, std::unique_ptr<joined_collection_resolver_producer>,
                         collection_composite_key_hash>;

  void register_joined_object_producer(const collection_composite_key &key, std::unique_ptr<abstract_object_resolver_producer> &&producer) {
    joined_object_producers_[key] = std::move(producer);
  }

  void register_collection_producer(const collection_composite_key &key, std::unique_ptr<joined_collection_resolver_producer> &&producer) {
    collection_producers_[key] = std::move(producer);
  }

  [[nodiscard]] const joined_object_producer_map &joined_object_producers() const {
    return joined_object_producers_;
  }

  [[nodiscard]] const collection_producer_map &collection_producers() const {
    return collection_producers_;
  }

private:
  joined_object_producer_map joined_object_producers_;
  collection_producer_map collection_producers_;
};

template <typename Type>
class object_resolver_producer final : public abstract_object_resolver_producer {
public:
  object_resolver_producer(const basic_schema &repo, const table &tab, const std::string &where_column, const std::type_index &root_type = typeid(void))
  : abstract_object_resolver_producer(typeid(Type), where_column, root_type)
  , repo_(repo)
  , table_(tab) {}

  void create_relation_producers(resolver_producer_registry &registry) const override;
  result<query_context, error> build_query(const dialect &d) override;

  std::shared_ptr<abstract_resolver> produce(statement &&stmt) const override {
    return std::make_shared<query_object_resolver<Type>>(std::move(stmt));
  }

private:
  const basic_schema &repo_;
  const table &table_;
};

template<typename Type>
class query_joined_collection_resolver_producer : public joined_collection_resolver_producer {
public:
  query_joined_collection_resolver_producer() = default;
  query_joined_collection_resolver_producer(const basic_schema& repo, const table& tab, const std::string& where_column,
                                            const std::string &join_column, const std::type_index& root_type)
  : joined_collection_resolver_producer(root_type, typeid(Type), where_column, join_column)
  , repo_(repo)
  , table_(tab) {}

  result<query_context, error> build_query(const dialect& d) override;

  std::shared_ptr<abstract_joined_resolver> produce(statement&& stmt, const resolver_service& rs) const override {
    return std::make_shared<query_collection_resolver<Type>>(std::move(stmt), root_type(), join_column(), rs.resolver<typename Type::value_type>());
  }

private:
  const basic_schema& repo_;
  const table& table_;
};

class primary_key_options;
class foreign_key_options;
class column_options;
class producer_creator final {
public:
  producer_creator(const basic_schema &schema, const std::type_index &root_type, resolver_producer_registry &registry)
  : schema_(schema), root_type_(root_type), registry_(registry) {}

  template <typename BaseType> static void on_base(const BaseType &) {}
  template <typename ValueType>
  static void on_primary_key(const char * /*id*/, ValueType & /*value*/, const primary_key_options & /*attr*/) {}
  static void on_revision(const char * /*id*/, uint64_t & /*rev*/) {}
  template <class Type>
  static void on_attribute(const char * /*id*/, Type & /*value*/, const column_options & /*attr*/) {}
  template <class Pointer>
  static void on_belongs_to(const char * /*id*/, Pointer & /*x*/, const foreign_key_options & /*attr*/) {}

  template <class PointerType>
  void on_has_one(const char * /*id*/, PointerType & /*x*/, const char *join_column, const foreign_key_options & /*attr*/) {
    const auto it = schema_.find(typeid(typename PointerType::value_type));
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }
    if (!it->info().has_primary_key()) {
      throw error_exception{error_code::MissingPrimaryKey, "Missing primary key"};
    }

    auto producer = std::make_unique<object_resolver_producer<typename PointerType::value_type>>(
        schema_, *it->info().table(), join_column, root_type_);

    const collection_composite_key key{root_type_, typeid(PointerType::value_type), join_column};
    registry_.register_joined_object_producer(key, std::move(producer));
  }

  template <class CollectionType>
  void on_has_many(
      const char * /*id*/, CollectionType & /*cont*/, const char *join_column,
      const foreign_key_options & /*attr*/,
      std::enable_if_t<is_object_ptr<typename CollectionType::value_type>::value> * = nullptr) {
    const auto it = schema_.find(typeid(typename CollectionType::value_type::value_type));
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }
    if (!it->info().has_primary_key()) {
      throw error_exception{error_code::MissingPrimaryKey, "Missing primary key"};
    }

    auto producer = std::make_unique<query_joined_collection_resolver_producer<typename CollectionType::value_type>>(
      schema_, *it->info().table(), it->info().primary_key_attribute()->column_name(), join_column, root_type_
    );

    const collection_composite_key key{root_type_, typeid(typename CollectionType::value_type), join_column};
    registry_.register_collection_producer(key, std::move(producer));
  }

  template <class CollectionType>
  void on_has_many(
      const char *id, CollectionType & /*cont*/, const char *join_column,
      const foreign_key_options & /*attr*/,
      std::enable_if_t<!is_object_ptr<typename CollectionType::value_type>::value> * = nullptr) {
    const auto it = schema_.find(id);
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type" + std::string{id}};
    }
    auto producer = std::make_unique<
        query_joined_collection_resolver_producer<typename CollectionType::value_type>>(
        schema_, *it->info().table(), "value", join_column, root_type_);

    const collection_composite_key key{root_type_, typeid(typename CollectionType::value_type),
                                       join_column};
    registry_.register_collection_producer(key, std::move(producer));
  }

  template <class CollectionType>
  void on_has_many_to_many(const char *id, CollectionType &, const char *join_column, const char *inverse_join_column, const foreign_key_options & /*attr*/) {
    const auto it = schema_.find(id);
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }

    auto producer = std::make_unique<
        query_joined_collection_resolver_producer<typename CollectionType::value_type>>(
        schema_, *it->info().table(), inverse_join_column, join_column, root_type_);
    const collection_composite_key key{root_type_, typeid(typename CollectionType::value_type),
                                       inverse_join_column};
    registry_.register_collection_producer(key, std::move(producer));
  }

  template <class CollectionType>
  void on_has_many_to_many(const char *id, CollectionType &, const foreign_key_options & /*attr*/) {
    const auto it = schema_.find(id);
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }

    join_columns_collector collector;
    const auto jc = collector.collect<typename CollectionType::value_type::value_type>();

    auto producer = std::make_unique<
        query_joined_collection_resolver_producer<typename CollectionType::value_type>>(
        schema_, *it->info().table(), jc.join_column, jc.inverse_join_column, root_type_);
    const collection_composite_key key{root_type_, typeid(typename CollectionType::value_type),
                                       jc.join_column};
    registry_.register_collection_producer(key, std::move(producer));
  }

private:
  const basic_schema &schema_;
  const std::type_index root_type_;
  resolver_producer_registry &registry_;
};

template <typename Type>
void object_resolver_producer<Type>::create_relation_producers(resolver_producer_registry &registry) const {
  producer_creator pc(repo_, typeid(Type), registry);
  Type obj;
  access::process(pc, obj);
}

template <typename Type>
result<query_context, error> object_resolver_producer<Type>::build_query(const dialect &d) {
  const auto *pk_column = table_[where_column()];
  select_query_builder qb(repo_);

  const auto result = qb.build<Type>(*pk_column == _);
  if (!result) {
    return failure(result.err());
  }

  return ok(result->compile(d));
}

template <typename Type>
result<query_context, error> query_joined_collection_resolver_producer<Type>::build_query(const dialect &d) {
  const auto *pk_column = table_[where_column()];
  const auto *join_column = table_[this->join_column()];

  const auto ctx = select({*pk_column})
    .from(table_)
    .where(*join_column == _)
    .compile(d);

  return ok(ctx);
}
} // namespace matador::query
#endif // MATADOR_PRODUCER_CREATOR_HPP
