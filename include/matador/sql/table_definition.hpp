#ifndef QUERY_TABLE_DEFINITION_HPP
#define QUERY_TABLE_DEFINITION_HPP

#include "matador/sql/column.hpp"
#include "matador/sql/column_definition.hpp"

#include <unordered_map>

namespace matador::sql {

class table_definition final
{
private:
  using column_by_index = std::vector<column_definition>;
  using column_index_pair = std::pair<std::reference_wrapper<column_definition>, column_by_index::difference_type>;
  using column_by_name_map = std::unordered_map<std::string, column_index_pair>;

public:
  using iterator = column_by_index::iterator;
  using const_iterator = column_by_index::const_iterator;

  table_definition() = default;
  table_definition(std::initializer_list<column_definition> columns);
  explicit table_definition(const std::vector<column_definition> &columns);
  table_definition(const table_definition &x);
  table_definition& operator=(const table_definition &x);
  table_definition(table_definition&&) noexcept = default;
  table_definition& operator=(table_definition&&) noexcept = default;
  ~table_definition() = default;

  [[nodiscard]] bool has_primary_key() const;
  [[nodiscard]] std::optional<column_definition> primary_key() const;

  template < typename Type >
  void append(const std::string &name, long size = -1)
  {
    append(make_column<Type>(name, size));
  }
  void append(column_definition col);

  [[nodiscard]] const std::vector<column_definition>& columns() const;

  [[nodiscard]] const column_definition& at(const column &col) const;
  [[nodiscard]] const column_definition& at(size_t index) const;

  iterator find(const std::string &column_name);
  [[nodiscard]] const_iterator find(const std::string &column_name) const;

  iterator begin();
  [[nodiscard]] const_iterator begin() const;
  [[nodiscard]] const_iterator cbegin() const;

  iterator end();
  [[nodiscard]] const_iterator end() const;
  [[nodiscard]] const_iterator cend() const;

  [[nodiscard]] size_t size() const;
  [[nodiscard]] bool empty() const;
  void clear();

private:
  void init();
  void add_to_map(column_definition &col, size_t index);

private:
  column_by_index columns_;
  column_by_name_map columns_by_name_;

  int pk_index_{-1};
};

}
#endif //QUERY_TABLE_DEFINITION_HPP
