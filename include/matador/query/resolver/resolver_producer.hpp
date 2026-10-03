#ifndef MATADOR_RESOLVER_PRODUCER_HPP
#define MATADOR_RESOLVER_PRODUCER_HPP

#include "matador/query/query_context.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

#include <memory>

namespace matador::query {
class dialect;
class statement;
class abstract_object_resolver_producer {
public:
  virtual ~abstract_object_resolver_producer() = default;
  virtual utils::result<query_context, utils::error> build_query(const dialect& d) = 0;
  virtual std::shared_ptr<abstract_resolver> produce(statement&& stmt) const = 0;

  [[nodiscard]] const std::type_index& type() const;

protected:
  explicit abstract_object_resolver_producer(const std::type_index &type);

private:
  std::type_index type_;
};

class abstract_joined_object_resolver_producer {
public:
  virtual ~abstract_joined_object_resolver_producer() = default;
  virtual utils::result<query_context, utils::error> build_query(const dialect& d) = 0;
  virtual std::shared_ptr<abstract_resolver> produce(statement&& stmt) const = 0;

  [[nodiscard]] const std::type_index& root_type() const;
  [[nodiscard]] const std::type_index& type() const;
  [[nodiscard]] const std::string& collection_name() const;

protected:
  explicit abstract_joined_object_resolver_producer(const std::type_index &root_type, const std::type_index &type, std::string collection_name);

private:
  std::type_index root_type_;
  std::type_index type_;
  std::string collection_name_;
};
}
#endif //MATADOR_RESOLVER_PRODUCER_HPP