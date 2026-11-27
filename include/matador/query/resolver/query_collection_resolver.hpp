#ifndef MATADOR_QUERY_CONTAINER_RESOLVER_HPP
#define MATADOR_QUERY_CONTAINER_RESOLVER_HPP

#include "matador/query/relations/collection_resolver.hpp"
#include "matador/query/relations/object_resolver.hpp"
#include "matador/query/internal/identifier_statement_binder.hpp"
#include "matador/query/runtime/statement.hpp"
#include "matador/utils/error_exception.hpp"

#include <mutex>

namespace matador::query {
class executor;
template<typename Type>
class query_collection_resolver : public collection_resolver<Type> {
public:
  using value_type = typename collection_resolver<Type>::value_type;

  explicit query_collection_resolver(statement &&stmt,
                                     const std::type_index& root_type,
                                     std::string join_column,
                                     const std::shared_ptr<object_resolver<typename Type::value_type>> &resolver = {})
  : collection_resolver<Type>(root_type, join_column)
  , stmt_(std::move(stmt))
  , resolver_(resolver)
  {}

  std::vector<value_type> resolve(const identifier &id) override;
protected:
  statement stmt_;
  std::mutex mutex_;
  std::type_index index{typeid(Type)};
  std::shared_ptr<object_resolver<typename Type::value_type>> resolver_;
};

struct value_to_identifier {
  void operator()(const int8_t &x) { assign(x); }
  void operator()(const int16_t &x) { assign(x); }
  void operator()(const int32_t &x) { assign(x); }
  void operator()(const int64_t &x) { assign(x); }
  void operator()(const uint8_t &x) { assign(x); }
  void operator()(const uint16_t &x) { assign(x); }
  void operator()(const uint32_t &x) { assign(x); }
  void operator()(const uint64_t &x) { assign(x); }
  void operator()(const bool &) {}
  void operator()(const float &) {}
  void operator()(const double &) {}
  void operator()(const char *) {}
  void operator()(const std::string &x) { assign(x); }
  void operator()(const utils::date_type_t &) {}
  void operator()(const utils::time_type_t &) {}
  void operator()(const utils::timestamp_type_t &) {}
  void operator()(const utils::blob_type_t &) {}

  template<typename Type>
  void assign(const Type &x) { id_ = x; }

  identifier id_;
};

struct identifier_creator {
  value_to_identifier visitor;
  void on_attribute(const char*, const column_value &val, const column_options &/*attr*/) {
    std::visit(visitor, val.raw_value());
  }
};

template < typename ItemType, typename Enable = void >
struct item_appender_trait;

template < typename ItemType >
struct item_appender_trait<ItemType, std::enable_if_t<is_object_ptr<ItemType>::value>> {
  static void append(std::vector<ItemType>& items, field& f, const std::shared_ptr<object_resolver<typename ItemType::value_type>>& resolver) {
    identifier_creator creator;
    f.process(creator);
    const auto op = std::make_shared<object_proxy<typename ItemType::value_type>>(resolver, creator.visitor.id_);
    items.emplace_back(op);
  }
};

template < typename ItemType >
struct item_appender_trait<ItemType, std::enable_if_t<!is_object_ptr<ItemType>::value>> {
  static void append(std::vector<ItemType>& items, const field& f, const std::shared_ptr<object_resolver<typename ItemType::value_type>>&) {
    auto val = f.as<ItemType>();
    if (val.has_value()) {
      items.push_back(*val);
    }
  }
};

template<typename Type>
std::vector<typename query_collection_resolver<Type>::value_type> query_collection_resolver<Type>::resolve(const identifier &id) {
  std::lock_guard lock(mutex_);
  identifier_statement_binder binder(stmt_);
  binder.bind(id);

  auto result = stmt_.fetch();
  if (!result) {
    throw error_exception(result.release_error());
  }
  std::vector<value_type> objects;
  identifier_creator creator;
  for (auto &item : *result) {
    if (item.size() != 1) {
      continue;
    }

    item_appender_trait<Type>::append(objects, item.at(0), resolver_);
  }
  return objects;
}
} // namespace matador::query

#endif //MATADOR_QUERY_CONTAINER_RESOLVER_HPP