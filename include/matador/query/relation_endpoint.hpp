#ifndef MATADOR_RELATION_ENDPOINT_HPP
#define MATADOR_RELATION_ENDPOINT_HPP

#include "matador/query/relation_type.hpp"

#include <memory>

namespace matador::query {
class schema_node;

class relation_endpoint {
public:
  relation_endpoint(std::string field_name, relation_type type, const schema_node& node);

  [[nodiscard]] std::string field_name() const;
  [[nodiscard]] relation_type type() const;
  [[nodiscard]] std::string type_name() const;
  [[nodiscard]] const schema_node& node() const;

  [[nodiscard]] bool is_has_one() const;
  [[nodiscard]] bool is_has_many() const;
  [[nodiscard]] bool is_belongs_to() const;

  [[nodiscard]] std::shared_ptr<relation_endpoint> foreign_endpoint() const;
  void link_foreign_endpoint(const std::shared_ptr<relation_endpoint>& endpoint);

private:
  template<typename Type, template<typename> typename... Observers>
  friend class relation_completer;
  template<typename Type, template<typename> typename ...Observers>
  friend class foreign_node_completer;

  std::string field_name_;
  relation_type type_;
  const schema_node *node_{nullptr};
  std::weak_ptr<relation_endpoint> foreign_endpoint_;
};
}
#endif //MATADOR_RELATION_ENDPOINT_HPP
