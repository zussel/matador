#ifndef MATADOR_INSERT_QUERY_BUILDER_HPP
#define MATADOR_INSERT_QUERY_BUILDER_HPP

#include "matador/query/basic_schema.hpp"
#include "matador/query/collection.hpp"
#include "matador/query/intermediates/executable_query.hpp"
#include "matador/query/internal/execute_step.hpp"
#include "matador/query/internal/insert_step.hpp"
#include "matador/query/internal/primary_key_accessor.hpp"
#include "matador/query/internal/query_contexts.hpp"
#include "matador/query/object_ptr.hpp"
#include "matador/query/query.hpp"
#include "matador/query/query_builder_utils.hpp"
#include "matador/query/statement.hpp"

#include "matador/utils/error_exception.hpp"

namespace matador::query {
class statement_cache;
}

namespace matador::query {
template < class ObjectType >
class has_many_linker {
public:
  has_many_linker(const object_ptr<ObjectType> &ptr, std::string join_column)
  : ptr_(ptr), join_column_(std::move(join_column)) {}
  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template < class PrimaryKeyType >
  static void on_primary_key(const char * /*id*/, PrimaryKeyType &, const primary_key_options& /*attr*/) {}
  static void on_revision(const char * /*id*/, uint64_t &/*rev*/) {}
  template<typename Type>
  static void on_attribute(const char * /*id*/, Type &, const column_options &/*attr*/) {}
  template<class Pointer>
  static void on_belongs_to(const char * /*id*/, Pointer &/*obj*/, const foreign_key_options &/*attr*/) {}
  void on_belongs_to(const char *id, object_ptr<ObjectType> &obj, const foreign_key_options &/*attr*/) {
    if (id != join_column_) {
      return;
    }

    obj = ptr_;
  }
  template<class Pointer>
  static void on_has_one(const char * /*id*/, Pointer &/*obj*/, const char * /*join_column*/, const foreign_key_options &/*attr*/) {}
  template<class CollectionType>
  static void on_has_many(const char * /*id*/, CollectionType &/*con*/, const char *, const foreign_key_options &/*attr*/, std::enable_if_t<!is_object_ptr<typename CollectionType::value_type>::value> * = nullptr) {}
  template<class Collection>
  static void on_has_many_to_many(const char * /*id*/, Collection &/*container*/, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_key_options & ) {}
  template<class Collection>
  static void on_has_many_to_many(const char * /*id*/, Collection &/*container*/, const foreign_key_options & ) {}

private:
  object_ptr<ObjectType> ptr_;
  std::string join_column_;
};

template < typename ObjectType >
class insert_step_processor {
public:
  explicit insert_step_processor(query_builder_context &ctx)
  : ctx_{ctx}
  {}

  utils::result<void, utils::error> build(object_ptr<ObjectType> ptr, const bool as_relation_step = false) {
    if (!ptr) {
      return utils::failure(utils::error{error_code::InvalidObject, "Object is null"});
    }
    ptr_ = ptr;

    const auto key = make_entity_visit_key<ObjectType>(*ptr_);
    if (ctx_.visited_.find(key) != ctx_.visited_.end()) {
      return utils::ok<void>();
    }
    ctx_.visited_.insert(key);

    const auto it = ctx_.schema_.find(typeid(ObjectType));
    if (it == ctx_.schema_.end()) {
      return utils::failure(utils::error{error_code::UnknownType, "Unknown type"});
    }

    // 1) Traverse relations first => dependencies will be inserted before this object
    try {
      access::process(*this, *ptr_);
    } catch (const error_exception &ex) {
      return utils::failure(ex.error());
    }

    // 2) Build INSERT for this object
    const auto &info = it->info();
    if (!info.has_primary_key() || it->second.pk_generator().type() == generator_type::None) {
      return utils::failure(utils::error{error_code::MissingPrimaryKey, "Type " + info.name() + " has no primary key"});
    }
    const auto cit = ctx_.contexts_by_type_.find(it->info().type_index());
    if (cit == ctx_.contexts_by_type_.end()) {
      return utils::failure(utils::error{error_code::UnknownType, "Unknown type"});
    }

    auto step = create_insert_step(cit->second.insert, it->second);
    if (as_relation_step) {
      ctx_.relation_steps_.push_back(std::move(step));
    } else {
      ctx_.steps_.push_back(std::move(step));
    }

    ptr_.reset();

    return utils::ok<void>();
  }

  template < class PrimaryKeyType >
  static void on_primary_key(const char * /*id*/, PrimaryKeyType &, const primary_key_options& /*attr*/) {}
  static void on_revision(const char * /*id*/, uint64_t &/*rev*/) {}
  template<typename Type>
  static void on_attribute(const char * /*id*/, Type &, const column_options &/*attr*/) {}

  template<class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer &obj, const foreign_key_options &attr) {
    on_foreign_object(obj, attr);
  }

  template<class Pointer>
  void on_has_one(const char * /*id*/, Pointer &obj, const char * /*join_column*/, const foreign_key_options &attr) {
    on_foreign_object(obj, attr);
  }

  template<class CollectionType>
  void on_has_many(const char * /*id*/,
                   collection<object_ptr<CollectionType>> &objects,
                   const char *join_column,
                   const foreign_key_options &attr) {
    if (join_column == nullptr) {
      return;
    }
    if (!utils::is_cascade_type_set(attr.cascade(), cascade_type::Insert)) {
      return;
    }

    has_many_linker<ObjectType> linker(ptr_, join_column);
    insert_step_processor<CollectionType> processor{ctx_};
    for (auto &obj : objects) {
      if (!obj) {
        continue;
      }

      if (obj.is_transient()) {
        auto result = processor.build(obj, true);
        if (!result) {
          throw error_exception(result.release_error());
        }
      }

      access::process(linker, *obj);
    }
  }

  template<class CollectionType>
  void on_has_many(const char *id,
                   collection<CollectionType> &objects,
                   const char *join_column,
                   const foreign_key_options &attr) {
    if (id == nullptr || join_column == nullptr) {
      return;
    }
    if (!utils::is_cascade_type_set(attr.cascade(), cascade_type::Insert)) {
      return;
    }

    const auto it = ctx_.schema_.find(std::string{id});
    if (it == ctx_.schema_.end()) {
      throw error_exception(error_code::UnknownType, "Unknown type " + std::string{id});
    }

    using relation_value_type = many_to_relation<ObjectType, CollectionType>;
    if (std::type_index(typeid(relation_value_type)) != it->info().type_index()) {
      throw error_exception(error_code::InvalidRelationType, "Invalid relation type");
    }

    const auto cit = ctx_.contexts_by_type_.find(it->info().type_index());
    if (cit == ctx_.contexts_by_type_.end()) {
      throw error_exception(error_code::UnknownType, "Unknown type" + std::string{id});
    }

    for (auto &obj : objects) {
      auto rel = make_object<relation_value_type>(join_column, "value", ptr_, obj);

      ctx_.relation_steps_.push_back(std::make_unique<insert_step_relation<relation_value_type>>(cit->second.insert, rel));
    }
  }

    template<class ForeignType>
    void on_has_many_to_many(const char *id,
                             collection<object_ptr<ForeignType>> &objects,
                             const char *join_column,
                             const char *inverse_join_column,
                             const foreign_key_options &attr) {
    if (id == nullptr || join_column == nullptr || inverse_join_column == nullptr) {
      return;
    }

    using relation_value_type = many_to_many_relation<ObjectType, ForeignType>;
    const std::type_index foreign_type{typeid(ForeignType)};
    const std::type_index local_type{typeid(ObjectType)};
    insert_many_to_many_relations<relation_value_type>(
      id,
      objects,
      attr,
      [foreign_type, local_type](const char* relation_name) -> processing_many_to_many_key {
        return {std::string{relation_name}, local_type, foreign_type};
      },
      [this, join_column, inverse_join_column](const auto &obj) {
        return make_object<relation_value_type>(join_column, inverse_join_column, ptr_, obj);
      });
  }

  template<class ForeignType>
  void on_has_many_to_many(const char *id, collection<object_ptr<ForeignType>> &objects, const foreign_key_options &attr) {
    if (id == nullptr) {
      return;
    }

    join_columns_collector collector;
    auto join_columns = collector.collect<ForeignType>();
    if (join_columns.join_column.empty() || join_columns.inverse_join_column.empty()) {
      return;
    }

    using relation_value_type = many_to_many_relation<ForeignType, ObjectType>;
    const std::type_index foreign_type{typeid(ForeignType)};
    const std::type_index local_type{typeid(ObjectType)};
    insert_many_to_many_relations<relation_value_type>(
      id,
      objects,
      attr,
      [foreign_type, local_type](const char* relation_name) -> processing_many_to_many_key {
        return {std::string{relation_name}, foreign_type, local_type};
      },
      [this, join_columns = std::move(join_columns)](const auto &obj) {
        return make_object<relation_value_type>(join_columns.inverse_join_column, join_columns.join_column, obj, ptr_);
      });
  }

private:
  template<class PointerType>
  void on_foreign_object(object_ptr<PointerType> &obj, const foreign_key_options &attr) {
    if (!utils::is_cascade_type_set(attr.cascade(), cascade_type::Insert) || !obj || !obj.is_transient()) {
      return;
    }

    insert_step_processor<PointerType> processor{ctx_};

    auto result = processor.build(obj);
    if (!result) {
      throw error_exception(result.release_error());
    }
  }

  template<class LocalType, class ForeignType, class RelationKeyFactory, class RelationFactory>
  void insert_many_to_many_relations(const char *id,
                                     collection<object_ptr<ForeignType>> &objects,
                                     const foreign_key_options &attr,
                                     RelationKeyFactory make_relation_key,
                                     RelationFactory make_relation) {
    if (!utils::is_cascade_type_set(attr.cascade(), cascade_type::Insert)) {
      return;
    }

    const auto key = make_relation_key(id);
    if (ctx_.processing_many_to_many_relations_.find(key) != ctx_.processing_many_to_many_relations_.end()) {
      return;
    }

    const auto it = ctx_.schema_.find(std::string{id});
    if (it == ctx_.schema_.end()) {
      throw error_exception(error_code::UnknownType, "Unknown type for relation " + std::string{id});
    }

    if (std::type_index(typeid(LocalType)) != it->info().type_index()) {
      throw error_exception(error_code::InvalidRelationType, "Invalid relation type for " + std::string{id});
    }

    const auto cit = ctx_.contexts_by_type_.find(it->info().type_index());
    if (cit == ctx_.contexts_by_type_.end()) {
      throw error_exception(error_code::UnknownType, "No query contexts for type " + it->name());
    }

    std::ignore = ctx_.processing_many_to_many_relations_.insert(key);
    std::vector<std::unique_ptr<execute_step>> insert_relation_steps;
    insert_step_processor<ForeignType> processor(ctx_);
    for (auto &obj : objects) {
      if (!obj) {
        continue;
      }

      if (obj.is_transient()) {
        auto result = processor.build(obj, true);
        if (!result) {
          throw error_exception(result.release_error());
        };
      }

      auto rel = make_relation(obj);

      // access::process(*this, *rel);

      insert_relation_steps.push_back(std::make_unique<insert_step_relation<LocalType>>(cit->second.insert, rel));
    }
    ctx_.relation_steps_.insert(
      ctx_.relation_steps_.end(),
      std::make_move_iterator(insert_relation_steps.begin()),
      std::make_move_iterator(insert_relation_steps.end()));
    ctx_.processing_many_to_many_relations_.erase(key);
  }

  std::unique_ptr<execute_step> create_insert_step(const query_context& query_ctx, const schema_node& node) {
    if (node.pk_generator().type() == generator_type::Manual) {
      return std::make_unique<insert_step_pk_manual<ObjectType>>(query_ctx, ptr_);
    }
    if (node.pk_generator().type() == generator_type::Identity) {
      return std::make_unique<insert_step_pk_identity<ObjectType>>(query_ctx, ptr_, node.node().info().primary_key_attribute()->name());
    }
    return std::make_unique<insert_step_pk_generated<ObjectType>>(query_ctx, ptr_, node.pk_generator());
  }

private:
  query_builder_context& ctx_;
  object_ptr<ObjectType> ptr_;
};

template<class ObjectType>
class insert_query_builder {
public:
  explicit insert_query_builder(const basic_schema &schema, const std::unordered_map<std::type_index, query_contexts> &contexts_by_type)
  : schema_(schema)
  , contexts_by_type_(contexts_by_type)
  {}

  utils::result<std::vector<std::unique_ptr<execute_step>>, utils::error> build(const object_ptr<ObjectType> &ptr) {
    if (const auto it = schema_.find(typeid(ObjectType)); it == schema_.end()) {
      return utils::failure(utils::error{error_code::UnknownType, "Unknown type for insert query"});
    }

    query_builder_context ctx{schema_, contexts_by_type_};
    insert_step_processor<ObjectType> processor{ctx};

    const auto result = processor.build(ptr);
    if (!result) {
      return utils::failure(result.err());
    }

    // relation inserts must run after all entity inserts were collected
    for (auto &s : ctx.relation_steps_) {
      ctx.steps_.push_back(std::move(s));
    }
    ctx.relation_steps_.clear();

    return utils::ok(std::move(ctx.steps_));
  }

private:
  const basic_schema &schema_;
  const std::unordered_map<std::type_index, query_contexts> &contexts_by_type_;
};
}
#endif //MATADOR_INSERT_QUERY_BUILDER_HPP