#ifndef MATADOR_DELETE_QUERY_BUILDER_HPP
#define MATADOR_DELETE_QUERY_BUILDER_HPP

#include "matador/query/collection.hpp"
#include "matador/query/object_cache.hpp"
#include "matador/query/object_ptr.hpp"
#include "matador/query/basic_schema.hpp"
#include "matador/query/error_code.hpp"
#include "matador/query/internal/delete_step.hpp"
#include "matador/query/internal/execute_step.hpp"
#include "matador/query/internal/query_contexts.hpp"
#include "matador/query/query_builder_utils.hpp"
#include "matador/query/identifier.hpp"
#include "matador/query/statement.hpp"
#include "matador/query/internal/primary_key_accessor.hpp"

#include "matador/utils/error_exception.hpp"
#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

namespace matador::query {
template<typename ObjectType>
class delete_step_processor {
public:
  explicit delete_step_processor(query_builder_context &ctx)
  : ctx_(ctx) {}

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

    if (const auto &info = it->info(); !info.has_primary_key()) {
      return utils::failure(utils::error{error_code::MissingPrimaryKey, "Type " + info.name() + " has no primary key"});
    }

    try {
      access::process(*this, *ptr_);
    } catch (const error_exception &ex) {
      return utils::failure(ex.error());
    }

    const auto cit = ctx_.contexts_by_type_.find(it->info().type_index());
    if (cit == ctx_.contexts_by_type_.end()) {
      return utils::failure(utils::error{error_code::UnknownType, "Unknown type"});
    }

    if (as_relation_step) {
      ctx_.relation_steps_.push_back(std::make_unique<delete_step_object<ObjectType>>(cit->second.delete_one, ptr_));
    } else {
      ctx_.steps_.push_back(std::make_unique<delete_step_object<ObjectType>>(cit->second.delete_one, ptr_));
    }

    ptr_.reset();

    return utils::ok<void>();
  }

  template<class PrimaryKeyType>
  static void on_primary_key(const char * /*id*/, PrimaryKeyType &, const primary_key_options & /*attr*/) {}

  static void on_revision(const char * /*id*/, uint64_t & /*rev*/) {}

  template<typename Type>
  static void on_attribute(const char * /*id*/, Type &, const column_options& /*attr*/) {}

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

    if (!utils::is_cascade_type_set(attr.cascade(), utils::cascade_type::Remove)) {
      return;
    }

    delete_step_processor<CollectionType> processor{ctx_};
    for (auto &obj : objects) {
      if (!obj) {
        continue;
      }

      auto result = processor.build(obj, true);
      if (!result) {
        throw error_exception(result.release_error());
      }
    }
  }

  template<class CollectionType>
  static void on_has_many(const char * /*id*/,
                          collection<CollectionType> & /*objects*/,
                          const char * /*join_column*/,
                          const foreign_key_options & /*attr*/) {
    // Value-Collections bzw. Relationstabellen werden hier nicht direkt gelöscht.
    // Dafür wird das Delete-Statement der jeweiligen Entity verwendet.
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
    on_many_to_many_objects<relation_value_type>(
      id,
      objects,
      attr,
      [foreign_type, local_type](const char* relation_name) -> processing_many_to_many_key {
        return {std::string{relation_name}, local_type, foreign_type};
      });
  }

  template<class ForeignType>
  void on_has_many_to_many(const char *id,
                           collection<object_ptr<ForeignType>> &objects,
                           const foreign_key_options &attr) {
    if (id == nullptr) {
      return;
    }

    join_columns_collector collector;
    if (auto join_columns = collector.collect<ForeignType>(); join_columns.join_column.empty() || join_columns.inverse_join_column.empty()) {
      return;
    }

    using relation_value_type = many_to_many_relation<ForeignType, ObjectType>;
    const std::type_index foreign_type{typeid(ForeignType)};
    const std::type_index local_type{typeid(ObjectType)};
    on_many_to_many_objects<relation_value_type>(
      id,
      objects,
      attr,
      [foreign_type, local_type](const char* relation_name) -> processing_many_to_many_key {
         return {std::string{relation_name}, foreign_type, local_type};
      });
  }

private:
  template<class PointerType>
  void on_foreign_object(object_ptr<PointerType> &obj, const foreign_key_options &attr) {
    if (!utils::is_cascade_type_set(attr.cascade(), utils::cascade_type::Remove) || !obj) {
      return;
    }

    delete_step_processor<PointerType> processor{ctx_};

    auto result = processor.build(obj);
    if (!result) {
      throw error_exception(result.release_error());
    }
  }

  template<class LocalType, class ForeignType, class RelationKeyFactory>
  void on_many_to_many_objects(const char *id,
                               collection<object_ptr<ForeignType>> &objects,
                               const foreign_key_options &attr,
                               RelationKeyFactory make_relation_key) {
    if (!utils::is_cascade_type_set(attr.cascade(), utils::cascade_type::Remove)) {
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

    if (const auto cit = ctx_.contexts_by_type_.find(it->info().type_index()); cit == ctx_.contexts_by_type_.end()) {
      throw error_exception(error_code::UnknownType, "No query contexts for type " + it->name());
    }

    std::ignore = ctx_.processing_many_to_many_relations_.insert(key);
    std::vector<std::unique_ptr<execute_step>> delete_relation_steps;
    delete_step_processor<ForeignType> processor{ctx_};
    for (auto &obj : objects) {
      if (!obj) {
        continue;
      }

      if (obj.is_persistent()) {
        auto result = processor.build(obj, true);
        if (!result) {
          throw error_exception(result.release_error());
        }
      }
    }
  }

private:
  query_builder_context &ctx_;
  object_ptr<ObjectType> ptr_;
};

template<class ObjectType>
class delete_query_builder {
public:
  explicit delete_query_builder(const basic_schema &schema,
                                const std::unordered_map<std::type_index, query_contexts> &contexts_by_type)
  : schema_(schema)
  , contexts_by_type_(contexts_by_type) {}

  utils::result<std::vector<std::unique_ptr<execute_step>>, utils::error> build(const object_ptr<ObjectType> &ptr) {
    if (const auto it = schema_.find(typeid(ObjectType)); it == schema_.end()) {
      return utils::failure(utils::error{error_code::UnknownType, "Unknown type for delete query"});
    }

    query_builder_context ctx{schema_, contexts_by_type_};
    delete_step_processor<ObjectType> processor{ctx};

    const auto result = processor.build(ptr);
    if (!result) {
      return utils::failure(result.err());
    }

    // relation inserts must run after all entity inserts were collected
    for (auto &s : ctx.steps_) {
      ctx.relation_steps_.push_back(std::move(s));
    }
    ctx.steps_.clear();

    return utils::ok(std::move(ctx.relation_steps_));
  }

private:
  const basic_schema &schema_;
  const std::unordered_map<std::type_index, query_contexts> &contexts_by_type_;
};
} // namespace matador::query

#endif //MATADOR_DELETE_QUERY_BUILDER_HPP
