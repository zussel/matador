#ifndef MATADOR_RESOLVER_PRODUCER_HPP
#define MATADOR_RESOLVER_PRODUCER_HPP

#include "matador/query/builder/query_context.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

#include <memory>

namespace matador::query {
class dialect;
class statement;
class resolver_producer_registry;
class abstract_object_resolver_producer {
public:
  virtual ~abstract_object_resolver_producer() = default;

  virtual void create_relation_producers(resolver_producer_registry &registry) const = 0;
  virtual result<query_context, error> build_query(const dialect& d) = 0;
  virtual std::shared_ptr<abstract_resolver> produce(statement&& stmt) const = 0;

  [[nodiscard]] const std::type_index& type() const;
  [[nodiscard]] const std::string& where_column() const;
  [[nodiscard]] const std::type_index& root_type() const;

protected:
  explicit abstract_object_resolver_producer(const std::type_index &type, std::string where_column, const std::type_index& root_type);

private:
  std::type_index type_;
  std::string where_column_;
  std::type_index root_type_;
};
}
#endif //MATADOR_RESOLVER_PRODUCER_HPP