#ifndef QUERY_QUERY_RESULT_IMPL_HPP
#define QUERY_QUERY_RESULT_IMPL_HPP

#include "matador/query/default_type_traits.hpp"
#include "matador/query/access.hpp"
#include "matador/query/column_options.hpp"
#include "matador/query/foreign_options.hpp"
#include "matador/query/primary_key_options.hpp"
#include "matador/query/identifier.hpp"

#include "matador/query/interface/query_result_reader.hpp"
#include "matador/query/internal/collection_utils.hpp"
#include "matador/query/internal/identifier_discover_service.hpp"
#include "matador/query/internal/primary_key_reader.hpp"
#include "matador/query/internal/identifier_reader.hpp"
#include "matador/query/resolver_service.hpp"
#include "matador/query/record.hpp"

#include "matador/query/column.hpp"
#include "matador/query/object_proxy.hpp"
#include "matador/query/object_ptr.hpp"
#include "matador/query/collection_proxy.hpp"
#include "matador/query/join_columns_collector.hpp"

#include <memory>
#include <stack>
#include <string>
#include <typeindex>
#include <unordered_set>

namespace matador::query {
class query_result_impl {
public:
  query_result_impl(std::unique_ptr<query_result_reader> &&reader,
                    std::vector<column> prototype,
                    const std::shared_ptr<resolver_service>& resolver,
                    const std::type_index& result_type,
                    size_t column_index = 0);

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &value, const primary_key_options& attr) {
    data_type_traits<ValueType>::read_value(*reader_, id, column_index_++, value, attr.size());
    if (type_stack_.size() == 1) {
      last_pk_ = current_pk_;
      current_pk_ = value;
    }
  }

  void on_revision(const char *id, uint64_t &rev);

  template<class Type>
  void on_attribute(const char *id, Type &x, const column_options &/*attr*/) {
    data_type_traits<Type>::read_value(*reader_, id, column_index_++, x);
  }

  void on_attribute(const char *id, char *value, const column_options &attr);
  void on_attribute(const char *id, std::string &value, const column_options &attr);
  void on_attribute(const char *id, column_value &val, const column_options &attr);

  template<class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer &x, const foreign_options &attr);
  template<class PointerType>
  void on_has_one(const char * /*id*/, object_ptr<PointerType> &x, const char * /*join_column*/, const foreign_options &attr);
  template<class CollectionType>
  void on_has_many(const char * /*id*/, CollectionType &cont, const char *join_column, const foreign_options &attr, std::enable_if_t<is_object_ptr<typename CollectionType::value_type>::value> * = nullptr);
  template<class CollectionType>
  void on_has_many(const char * /*id*/, CollectionType &, const char * /*join_column*/, const foreign_options &/*attr*/, std::enable_if_t<!is_object_ptr<typename CollectionType::value_type>::value> * = nullptr);
  template <class CollectionType>
  void on_has_many_to_many(const char *id, CollectionType &, const char * /*join_column*/, const char * /*inverse_join_column*/, const foreign_options &attr);
  template<class CollectionType>
  void on_has_many_to_many(const char *, CollectionType &, const foreign_options &attr);

  template<class Type>
  void bind(const Type &obj) {
    reader_->bind(obj);
  }

  [[nodiscard]] bool pk_has_changed() const {
    return !last_pk_.is_null() && last_pk_ != current_pk_;
  }

  template<class Type>
  bool fetch(Type &obj);
  bool fetch(record &rec);

  [[nodiscard]] const std::vector<column> &prototype() const;

private:
  template<class Type>
  identifier discover_current_primary_key(const Type &obj) {
    identifier_discover_service resolver(*reader_);
    return resolver.discover(obj);
  }

protected:
  size_t column_index_ = 0;
  std::vector<column> prototype_;
  std::unique_ptr<query_result_reader> reader_;
  std::shared_ptr<resolver_service> resolver_;
  const std::type_index result_type_;
  identifier_reader id_reader_;
  primary_key_reader pk_reader_;
  std::stack<std::type_index> type_stack_;
  identifier current_pk_{};
  identifier last_pk_{};
  std::unordered_set<collection_composite_key, collection_composite_key_hash> initialized_collections_;
};

template <class PointerType>
void query_result_impl::on_belongs_to(const char*, PointerType& x, const foreign_options& attr) {
  const auto resolver = resolver_->resolver<typename PointerType::value_type>();
  if (attr.fetch() == fetch_type::Lazy) {
    typename PointerType::value_type obj;
    auto pk = id_reader_.read(obj, column_index_++);
    x = PointerType(std::make_shared<object_proxy<typename PointerType::value_type>>(resolver, pk));
  } else {
    auto obj = std::make_shared<typename PointerType::value_type>();
    const auto ti = std::type_index(typeid(typename PointerType::value_type));
    type_stack_.push(ti);
    access::process(*this, *obj);
    type_stack_.pop();
    x = PointerType(std::make_shared<object_proxy<typename PointerType::value_type>>(resolver, obj));
  }
}

template <class PointerType>
void query_result_impl::on_has_one(const char*, object_ptr<PointerType>& x, const char *join_column, const foreign_options& attr) {
  const auto resolver = resolver_->joined_object_resolver<PointerType>(result_type_, join_column);
  if (attr.fetch() == fetch_type::Lazy) {
    x.reset(std::make_shared<object_proxy<PointerType>>(resolver, current_pk_));
  } else {
    auto obj = std::make_shared<PointerType>();
    const auto ti = std::type_index(typeid(typename PointerType::value_type));
    type_stack_.push(ti);
    access::process(*this, *obj);
    type_stack_.pop();
    x.reset(std::make_shared<object_proxy<PointerType>>(resolver, obj));
  }
}

template <class CollectionType>
void query_result_impl::on_has_many(const char *, CollectionType &cont, const char *join_column, const foreign_options &attr, std::enable_if_t<is_object_ptr<typename CollectionType::value_type>::value> *) {
  using value_type = typename CollectionType::value_type::value_type;
  auto object_resolver = resolver_->resolver<value_type>();
  auto resolver = resolver_->joined_collection_resolver<typename CollectionType::value_type>(result_type_, join_column);

  if (attr.fetch() == fetch_type::Lazy) {
    cont.reset(std::make_shared<collection_proxy<typename CollectionType::value_type>>(resolver, current_pk_));
  } else {
    if (initialized_collections_.insert({result_type_, typeid(typename CollectionType::value_type), std::string{join_column}}).second) {
      cont.reset(std::make_shared<collection_proxy<typename CollectionType::value_type>>(resolver, std::vector<typename CollectionType::value_type>()));
    }

    const auto ti = std::type_index(typeid(value_type));
    type_stack_.push(ti);
    auto obj = std::make_shared<typename CollectionType::value_type::value_type>();
    access::process(*this, *obj);
    type_stack_.pop();
    auto ptr = typename CollectionType::value_type(std::make_shared<object_proxy<value_type>>(object_resolver, obj));
    if (ptr.primary_key().is_valid()) {
      cont.push_back(ptr);
    }
  }
}
template <class CollectionType>
void query_result_impl::on_has_many(const char *id, CollectionType &cont, const char *join_column, const foreign_options &attr, std::enable_if_t<!is_object_ptr<typename CollectionType::value_type>::value> *) {
  using value_type = typename CollectionType::value_type;
  auto object_resolver = resolver_->resolver<value_type>();
  auto resolver = resolver_->joined_collection_resolver<value_type>(result_type_, join_column);

  if (attr.fetch() == fetch_type::Lazy) {
    cont.reset(std::make_shared<collection_proxy<value_type>>(resolver, current_pk_));
  } else {
    if (initialized_collections_.insert({result_type_, typeid(value_type), std::string{join_column}}).second) {
      cont.reset(std::make_shared<collection_proxy<value_type>>(resolver, std::vector<value_type>()));
    }

    // read a single value
    value_type value;
    data_type_traits<value_type>::read_value(*reader_, id, column_index_++, value, sizeof(value_type));
    cont.push_back(value);
  }
}

template <class CollectionType>
void query_result_impl::on_has_many_to_many(const char *id, CollectionType &cont, const char *join_column, const char *, const foreign_options &attr) {
  using value_type = typename CollectionType::value_type::value_type;
  auto object_resolver = resolver_->resolver<value_type>();
  auto resolver = resolver_->joined_collection_resolver<typename CollectionType::value_type>(result_type_, join_column);
  if (attr.fetch() == fetch_type::Lazy) {
    cont.reset(std::make_shared<collection_proxy<typename CollectionType::value_type>>(resolver, current_pk_));
  } else {
    if (initialized_collections_.insert({result_type_, typeid(typename CollectionType::value_type), std::string{id}}).second) {
      cont.reset(std::make_shared<collection_proxy<typename CollectionType::value_type>>(resolver, std::vector<typename CollectionType::value_type>()));
    }

    const auto ti = std::type_index(typeid(value_type));
    type_stack_.push(ti);
    auto obj = std::make_shared<typename CollectionType::value_type::value_type>();
    access::process(*this, *obj);
    type_stack_.pop();
    auto ptr = typename CollectionType::value_type(std::make_shared<object_proxy<value_type>>(object_resolver, obj));
    if (ptr.primary_key().is_valid()) {
      cont.push_back(ptr);
    }
  }
}

template <class CollectionType>
void query_result_impl::on_has_many_to_many(const char *id, CollectionType &cont, const foreign_options &attr) {
  using value_type = typename CollectionType::value_type::value_type;
  join_columns_collector collector;
  const auto jc = collector.collect<typename CollectionType::value_type::value_type>();

  auto object_resolver = resolver_->resolver<value_type>();
  auto resolver = resolver_->joined_collection_resolver<typename CollectionType::value_type>(result_type_, jc.inverse_join_column);
  if (attr.fetch() == fetch_type::Lazy) {
    cont.reset(std::make_shared<collection_proxy<typename CollectionType::value_type>>(resolver, current_pk_));
  } else {
    if (initialized_collections_.insert({result_type_, typeid(typename CollectionType::value_type), std::string{id}}).second) {
      cont.reset(std::make_shared<collection_proxy<typename CollectionType::value_type>>(resolver, std::vector<typename CollectionType::value_type>()));
    }

    const auto ti = std::type_index(typeid(value_type));
    type_stack_.push(ti);
    auto obj = std::make_shared<typename CollectionType::value_type::value_type>();
    access::process(*this, *obj);
    type_stack_.pop();
    auto ptr = typename CollectionType::value_type(std::make_shared<object_proxy<value_type>>(object_resolver, obj));
    if (ptr.primary_key().is_valid()) {
      cont.push_back(ptr);
    }
  }
}

template<class Type>
bool query_result_impl::fetch(Type &obj) {
  bool first = true;
  do {
    if (auto fetched = reader_->fetch(); !fetched.is_ok() || !*fetched) {
      return !first;
    }
    last_pk_ = current_pk_;
    current_pk_ = discover_current_primary_key(obj);
    if (pk_has_changed()) {
      reader_->unshift();
      last_pk_.clear();
      current_pk_.clear();
      break;
    }
    first = false;
    type_stack_.emplace(typeid(Type));
    column_index_ = reader_->start_column_index();
    access::process(*this, obj);
    type_stack_.pop();
  } while (last_pk_ == current_pk_);
  return true;
}
}

#endif //QUERY_QUERY_RESULT_IMPL_HPP
