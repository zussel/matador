#ifndef MATADOR_COLLECTION_RESOLVER_ATTACHER_HPP
#define MATADOR_COLLECTION_RESOLVER_ATTACHER_HPP

#include "matador/query/mapping/access.hpp"
#include "matador/query/relations/collection.hpp"
#include "matador/query/resolver_service.hpp"
#include "matador/query/runtime/error_code.hpp"
#include "matador/query/relations/join_columns_collector.hpp"

namespace matador::query {
class collection_resolver_attacher {
public:
  template<typename ObjectType>
  result<void, error> attach(ObjectType& object, const identifier& pk, const resolver_service& service) {
    pk_ = pk;
    owner_type_ = typeid(ObjectType);
    service_ = &service;
    missing_resolver_ = false;
    access::process(*this, object);
    if (missing_resolver_) {
      return failure(error(error_code::UnknownType, "Missing collection resolver for inserted type."));
    }
    return ok<void>();
  }

  template<typename BaseType> static void on_base(const BaseType&) {}
  template<typename T> static void on_primary_key(const char*, T&, const primary_key_options&) {}
  static void on_revision(const char*, uint64_t&) {}
  template<typename T> static void on_attribute(const char*, T&, const column_options&) {}
  template<typename T> static void on_belongs_to(const char*, T&, const foreign_key_options&) {}
  template<typename PointerType> void on_has_one(const char*, PointerType& ptr, const char*, const foreign_key_options&) {
    auto resolver = service_->resolver<typename PointerType::value_type>();
    if (!resolver) {
      missing_resolver_ = true;
      return;
    }

    ptr.proxy()->resolver(resolver);
  }

  template<typename T>
  void on_has_many(const char*, collection<object_ptr<T>>& objects, const char* join_column, const foreign_key_options& options) {
    if (options.fetch() != fetch_type::Lazy) {
      return;
    }
    auto resolver = service_->joined_collection_resolver<object_ptr<T>>(owner_type_, join_column);
    if (!resolver) {
      missing_resolver_ = true;
      return;
    }
    objects.proxy()->attach(resolver, pk_);
  }

  template<typename T> static void on_has_many(const char*, T&, const char*, const foreign_key_options&) {}
  template<typename T> static void on_has_many(const char*, T&, const foreign_key_options&) {}
  template<typename T> void on_has_many_to_many(const char*, collection<object_ptr<T>>& objects, const char *join_column, const char*, const foreign_key_options& options) {
    auto resolver = service_->joined_collection_resolver<object_ptr<T>>(owner_type_, join_column);
    if (!resolver) {
      missing_resolver_ = true;
      return;
    }
    objects.proxy()->attach(resolver, pk_);
  }
  template<typename T> void on_has_many_to_many(const char*, collection<object_ptr<T>>& objects, const foreign_key_options& options) {
    join_columns_collector collector;
    const auto jc = collector.collect<typename collection<object_ptr<T>>::value_type::value_type>();
    auto resolver = service_->joined_collection_resolver<object_ptr<T>>(owner_type_, jc.inverse_join_column);
    if (!resolver) {
      missing_resolver_ = true;
      return;
    }
    objects.proxy()->attach(resolver, pk_);
  }

private:
  identifier pk_{};
  std::type_index owner_type_{typeid(void)};
  const resolver_service* service_{};
  bool missing_resolver_{};
};
}
#endif // MATADOR_COLLECTION_RESOLVER_ATTACHER_HPP