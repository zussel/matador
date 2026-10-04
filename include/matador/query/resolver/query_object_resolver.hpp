#ifndef MATADOR_QUERY_OBJECT_LOADER_HPP
#define MATADOR_QUERY_OBJECT_LOADER_HPP

#include "matador/query/internal/identifier_statement_binder.hpp"
#include "matador/query/statement.hpp"

#include "matador/query/object_resolver.hpp"
#include "matador/utils/error_exception.hpp"

#include <mutex>

namespace matador::query {
class executor;
template<typename Type>
class query_object_resolver : public object_resolver<Type> {
public:
  explicit query_object_resolver(statement &&stmt)
  : stmt_(std::move(stmt)) {}

  std::shared_ptr<Type> resolve(const identifier &id) override;
protected:
  statement stmt_;
  std::mutex mutex_;
};

template<typename Type>
class query_joined_object_resolver : public joined_object_resolver<Type> {
public:
  explicit query_joined_object_resolver(const std::type_index& root_type, const std::string& join_column, statement &&stmt)
  : joined_object_resolver<Type>(root_type, join_column)
  , stmt_(std::move(stmt)) {}

  std::shared_ptr<Type> resolve(const identifier &id) override;
protected:
  statement stmt_;
  std::mutex mutex_;
};

template<typename Type>
std::shared_ptr<Type> query_object_resolver<Type>::resolve(const identifier &id) {
  std::lock_guard lock(mutex_);
  identifier_statement_binder binder(stmt_);
  binder.bind(id);

  auto result = stmt_.template fetch_one_raw<Type>();
  if (!result) {
    throw error_exception(result.release_error());
  }
  return *result;
}

template<typename Type>
std::shared_ptr<Type> query_joined_object_resolver<Type>::resolve(const identifier &id) {
  std::lock_guard lock(mutex_);
  identifier_statement_binder binder(stmt_);
  binder.bind(id);

  auto result = stmt_.template fetch_one_raw<Type>();
  if (!result) {
    throw error_exception(result.release_error());
  }
  return *result;
}
}
#endif //MATADOR_QUERY_OBJECT_LOADER_HPP