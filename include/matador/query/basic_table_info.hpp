#ifndef MATADOR_BASIC_TABLE_INFO_HPP
#define MATADOR_BASIC_TABLE_INFO_HPP

#include "matador/query/column.hpp"
#include "matador/query/constraint.hpp"
#include "matador/query/identifier.hpp"
#include "matador/query/relation_endpoint.hpp"
#include "matador/query/table.hpp"

namespace matador::query {
class schema_node;

class basic_table_info {
public:
  using t_endpoint_map = std::unordered_map<std::type_index, std::shared_ptr<relation_endpoint>>;
  using endpoint_iterator = t_endpoint_map::iterator;
  using const_endpoint_iterator = t_endpoint_map::const_iterator;

  virtual ~basic_table_info() = default;

  [[nodiscard]] std::type_index type_index() const;
  [[nodiscard]] std::string name() const;
  [[nodiscard]] std::shared_ptr<class table> table() const;
  [[nodiscard]] const std::vector<column>& columns() const;
  [[nodiscard]] const std::vector<constraint> constraints() const;

  [[nodiscard]] bool has_primary_key() const;
  // [[nodiscard]] const identifier& primary_key() const;
  [[nodiscard]] const column* primary_key_attribute() const;

  void update_name(const std::string& name) const;

  virtual void on_attach() const = 0;
  virtual void on_detach() const = 0;

  endpoint_iterator register_relation_endpoint(const std::type_index &type, const std::shared_ptr<relation_endpoint> &endpoint);
  void unregister_relation_endpoint(const std::type_index &type);

  [[nodiscard]] const_endpoint_iterator find_relation_endpoint(const std::type_index &type) const;
  endpoint_iterator find_relation_endpoint(const std::type_index &type);

  [[nodiscard]] const_endpoint_iterator find_relation_endpoint(const std::string &field) const;
  endpoint_iterator find_relation_endpoint(const std::string &field);

  endpoint_iterator endpoint_begin();
  [[nodiscard]] const_endpoint_iterator endpoint_begin() const;

  endpoint_iterator endpoint_end();
  [[nodiscard]] const_endpoint_iterator endpoint_end() const;

  [[nodiscard]] std::size_t endpoints_size() const;
  [[nodiscard]] bool endpoints_empty() const;

protected:
  basic_table_info(const schema_node& node, const std::shared_ptr<class table> &t);

protected:
  std::shared_ptr<class table> table_;
  const schema_node& node_;            /**< prototype node of the represented object type */
  t_endpoint_map relation_endpoints_;
};

using basic_table_info_ref = std::reference_wrapper<const basic_table_info>;

}
#endif //MATADOR_BASIC_TABLE_INFO_HPP
