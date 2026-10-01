#ifndef MATADOR_PRODUCER_CREATOR_HPP
#define MATADOR_PRODUCER_CREATOR_HPP


namespace matador::query {
class producer_creator final {
public:
  producer_creator(basic_schema& schema, const std::type_index& root_type)
  : schema_(schema)
  , root_type_(root_type)
  {}

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template<typename ValueType>
  static void on_primary_key(const char * /*id*/, ValueType &/*value*/, const utils::primary_key_attribute& /*attr*/) {}
  static void on_revision(const char * /*id*/, uint64_t &/*rev*/) {}
  template<class Type>
  static void on_attribute(const char * /*id*/, Type &/*value*/, const utils::field_attributes &/*attr*/) {}
  template<class Pointer>
  static void on_belongs_to(const char * /*id*/, Pointer & /*x*/, const utils::foreign_attributes &/*attr*/) {}
  template<class Pointer>
  void on_has_one(const char * /*id*/, Pointer & /*x*/, const char *join_column, const utils::foreign_attributes &/*attr*/) {
    const auto it = schema_.find(typeid(typename Pointer::value_type));
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }
    if (!it->second.node().info().has_primary_key()) {
      throw error_exception{error_code::MissingPrimaryKey, "Missing primary key"};
    }

    auto producer = std::make_unique<query_joined_object_resolver_producer<typename Pointer::value_type>>(
      schema_,
      it->second.table(),
      it->second.node().info().primary_key_attribute()->name(),
      root_type_,
      join_column);
    const object::collection_composite_key key{root_type_, typeid(Pointer), join_column};
    schema_.joined_object_resolver_producers_[key] = std::move(producer);
  }

  template<class CollectionType>
  void on_has_many(const char * /*id*/, CollectionType &/*cont*/, const char *join_column, const utils::foreign_attributes &/*attr*/, std::enable_if_t<object::is_object_ptr<typename CollectionType::value_type>::value> * = nullptr) {
    const auto it = schema_.find(typeid(typename CollectionType::value_type::value_type));
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }
    if (!it->second.node().info().has_primary_key()) {
      throw error_exception{error_code::MissingPrimaryKey, "Missing primary key"};
    }

    auto producer = std::make_unique<query_joined_collection_resolver_producer<typename CollectionType::value_type>>(
      schema_,
      it->second.table(),
      it->second.node().info().primary_key_attribute()->name(),
      root_type_,
      join_column);
    const object::collection_composite_key key{root_type_, typeid(typename CollectionType::value_type), join_column};
    schema_.collection_resolver_producers_[key] = std::move(producer);
  }

  template<class CollectionType>
  void on_has_many(const char *id, CollectionType &/*cont*/, const char *join_column, const utils::foreign_attributes &/*attr*/, std::enable_if_t<!object::is_object_ptr<typename CollectionType::value_type>::value> * = nullptr) {
    const auto it = schema_.find(id);
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type" + std::string{id}};
    }
    auto producer = std::make_unique<query_joined_collection_primitive_resolver_producer<typename CollectionType::value_type>>(
      schema_,
      it->second.table(),
      "value",
      root_type_,
      join_column);
    const object::collection_composite_key key{root_type_, typeid(typename CollectionType::value_type), join_column};
    schema_.collection_resolver_producers_[key] = std::move(producer);
  }

  template<class CollectionType>
  void on_has_many_to_many(const char *id, CollectionType &, const char *join_column, const char *inverse_join_column, const utils::foreign_attributes &/*attr*/) {
    const auto it = schema_.find(id);
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }

    auto producer = std::make_unique<query_joined_collection_resolver_producer<typename CollectionType::value_type>>(
      schema_,
      it->second.table(),
      inverse_join_column,
      root_type_,
      join_column);
    const object::collection_composite_key key{root_type_, typeid(typename CollectionType::value_type), inverse_join_column};
    schema_.collection_resolver_producers_[key] = std::move(producer);
  }

  template<class CollectionType>
  void on_has_many_to_many(const char *id, CollectionType &, const utils::foreign_attributes &/*attr*/) {
    const auto it = schema_.find(id);
    if (it == schema_.end()) {
      throw error_exception{error_code::UnknownType, "Unknown type"};
    }

    object::join_columns_collector collector;
    const auto jc = collector.collect<typename CollectionType::value_type::value_type>();

    auto producer = std::make_unique<query_joined_collection_resolver_producer<typename CollectionType::value_type>>(
      schema_,
      it->second.table(),
      jc.join_column,
      root_type_,
      jc.inverse_join_column);
    const object::collection_composite_key key{root_type_, typeid(typename CollectionType::value_type), jc.join_column};
    schema_.collection_resolver_producers_[key] = std::move(producer);

  }

private:
  basic_schema& schema_;
  const std::type_index root_type_;
};
}
#endif // MATADOR_PRODUCER_CREATOR_HPP
