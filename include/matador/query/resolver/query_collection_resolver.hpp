#ifndef MATADOR_QUERY_CONTAINER_RESOLVER_HPP
#define MATADOR_QUERY_CONTAINER_RESOLVER_HPP

#include "matador/query/collection_resolver.hpp"
#include "matador/query/object_resolver.hpp"
#include "matador/query/internal/identifier_statement_binder.hpp"
#include "matador/query/statement.hpp"

namespace matador::query {
class executor;
template<typename Type>
class query_collection_resolver : public collection_resolver<Type> {
public:
  using value_type = typename collection_resolver<Type>::value_type;

  explicit query_collection_resolver(statement &&stmt,
                                     const std::type_index& root_type,
                                     std::string join_column,
                                     const std::shared_ptr<object_resolver<typename Type::value_type>> &resolver)
  : collection_resolver<Type>(root_type, join_column)
  , stmt_(std::move(stmt))
  , resolver_(resolver)
  {}

  std::vector<value_type> resolve(const identifier &id) override;
protected:
  statement stmt_;
  std::type_index index{typeid(Type)};
  std::shared_ptr<object_resolver<typename Type::value_type>> resolver_;
};

template<typename Type>
class query_collection_primitive_resolver : public collection_resolver<Type> {
public:
  using value_type = typename collection_resolver<Type>::value_type;

  explicit query_collection_primitive_resolver(statement &&stmt,
                                               std::string join_column)
  : collection_resolver<Type>(join_column)
  , stmt_(std::move(stmt)) {}

  std::vector<value_type> resolve(const identifier &id) override {
    identifier_statement_binder binder(stmt_);
    binder.bind(id);

    auto result = stmt_.fetch();
    if (!result) {
      return {};
    }
    std::vector<Type> out;
    for (auto &r : *result) {
      if (r.size() != 1) {
        continue;
      }
      auto val = r.at<Type>(0);
      if (val.has_value()) {
        out.push_back({this->owner_, *val});
      }
    }
    return out;
  }
protected:
  statement stmt_;
  std::type_index index{typeid(Type)};
};

struct value_to_identifier{
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

template<typename Type>
std::vector<typename query_collection_resolver<Type>::value_type> query_collection_resolver<Type>::resolve(const identifier &id) {
  identifier_statement_binder binder(stmt_);
  binder.bind(id);

  auto result = stmt_.fetch();
  if (!result) {
    return {};
  }
  std::vector<value_type> out;
  for (auto &r : *result) {
    if (r.size() != 1) {
      continue;
    }
    identifier_creator creator;
    r.at(0).process(creator);
    const auto op = std::make_shared<object_proxy<typename Type::value_type>>(resolver_, creator.visitor.id_);
    out.emplace_back(op);
    // out.emplace_back({this->owner_, op});
  }
  return out;
}
}

#endif //MATADOR_QUERY_CONTAINER_RESOLVER_HPP