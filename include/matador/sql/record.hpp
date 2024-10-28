#ifndef QUERY_RECORD_HPP
#define QUERY_RECORD_HPP

#include "matador/utils/access.hpp"

#include "matador/sql/field.hpp"

#include <vector>
#include <unordered_map>

namespace matador::sql {

struct column;

class record
{
private:
  using field_ref = std::reference_wrapper<field>;
  using field_by_index = std::vector<field_ref>;
  using field_index_pair = std::pair<field, field_by_index::difference_type>;
  using field_by_name_map = std::unordered_map<std::string, field_index_pair>;

public:
  using iterator = field_by_index::iterator;
  using const_iterator = field_by_index::const_iterator;

  record() = default;
  record(std::initializer_list<field> columns);
  explicit record(const std::vector<field> &columns);
  record(const record &x);
  record& operator=(const record &x);
  record(record&&) noexcept = default;
  record& operator=(record&&) noexcept = default;
  ~record() = default;

  template<class Operator>
  void process(Operator &op)
  {
    for(auto &f : fields_) {
      f.get().process(op);
    }
  }

  void append(const field &col);

  [[nodiscard]] const std::vector<field_ref>& columns() const;

  [[nodiscard]] const field& at(const column &col) const;
  [[nodiscard]] const field& at(size_t index) const;
  template<class Type>
  std::optional<Type> at(const column &col) const
  {
    return at(col).as<Type>();
  }
  template<class Type>
  std::optional<Type> at(size_t index) const
  {
    return at(index).as<Type>();
  }

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

//  [[nodiscard]] bool unknown() const;

private:
  void init();
  void add_to_map(field &col, size_t index);

private:
  field_by_index fields_;
  field_by_name_map fields_by_name_;

//  int pk_index_{-1};
};

}
#endif //QUERY_RECORD_HPP
