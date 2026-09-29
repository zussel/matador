#ifndef MATADOR_RESOLVER_FACTORY_HPP
#define MATADOR_RESOLVER_FACTORY_HPP

#include "matador/query/resolver/abstract_joined_resolver.hpp"
#include "matador/query/resolver/object_resolver_factory.hpp"
#include "matador/query/resolver/joined_collection_resolver_factory.hpp"
#include "matador/query/internal/collection_utils.hpp"

#include <memory>
#include <unordered_map>

namespace matador::query {
class executor;

class producer_object_resolver_factory : public object_resolver_factory {
public:
  [[nodiscard]] std::shared_ptr<abstract_type_resolver> acquire_object_resolver(const std::type_index &type) const override;
  void register_object_resolver(std::shared_ptr<abstract_type_resolver> &&resolver) override;

private:
  std::unordered_map<std::type_index, std::shared_ptr<abstract_type_resolver>> resolvers_;
};

class producer_joined_collection_resolver_factory : public joined_collection_resolver_factory {
public:
  [[nodiscard]] std::shared_ptr<abstract_joined_resolver> acquire_collection_resolver(const std::type_index& root_type,
                                                                                                  const std::type_index& element_type,
                                                                                                  const std::string& collection_name) const override;
  void register_collection_resolver(std::shared_ptr<abstract_joined_resolver>&& resolver) override;

private:
  std::unordered_map<collection_composite_key, std::shared_ptr<abstract_joined_resolver>, collection_composite_key_hash> resolvers_;
};

class producer_joined_object_resolver_factory : public joined_object_resolver_factory {
public:
  [[nodiscard]] std::shared_ptr<abstract_type_resolver> acquire_joined_object_resolver(const std::type_index& root_type,
                                                                                                 const std::type_index& element_type,
                                                                                                 const std::string& collection_name) const override;
  void register_joined_object_resolver(std::shared_ptr<abstract_type_resolver>&& resolver, const std::type_index& root_type, const std::string& join_column) override;

private:
  std::unordered_map<collection_composite_key, std::shared_ptr<abstract_type_resolver>, collection_composite_key_hash> resolvers_;
};
}
#endif //MATADOR_RESOLVER_FACTORY_HPP