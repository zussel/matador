#ifndef MATADOR_COLLECTION_RESOLVER_PRODUCER_HPP
#define MATADOR_COLLECTION_RESOLVER_PRODUCER_HPP

#include "matador/query/resolver/abstract_joined_resolver.hpp"

#include "matador/query/query_context.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

#include <memory>

namespace matador::query {
class dialect;
class statement;
class resolver_service;
class joined_collection_resolver_producer {
public:
  virtual ~joined_collection_resolver_producer() = default;
  virtual utils::result<query_context, utils::error> build_query(const dialect& d) = 0;
  virtual std::shared_ptr<abstract_joined_resolver> produce(statement&& stmt, const resolver_service& rs) const = 0;

  [[nodiscard]] const std::type_index& root_type() const;
  [[nodiscard]] const std::type_index& type() const;
  [[nodiscard]] const std::string& join_column_name() const;

protected:
  explicit joined_collection_resolver_producer(const std::type_index &root_type, const std::type_index &type, std::string join_column_name);

private:
  std::type_index root_type_;
  std::type_index type_;
  std::string join_column_name_;
};
}
#endif  // MATADOR_COLLECTION_RESOLVER_PRODUCER_HPP
