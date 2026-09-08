#ifndef QUERY_RECORD_HPP
#define QUERY_RECORD_HPP

#include "matador/query/field.hpp"

#include <vector>
#include <unordered_map>

namespace matador::query {

class record final {
public:
  using iterator = std::vector<field>::iterator;
  using const_iterator = std::vector<field>::const_iterator;

  record() = default;
  record(std::initializer_list<field> columns);
  explicit record(const std::vector<field> &columns);
  record(const record &x) = default;
  record& operator=(const record &x);
  record(record&&) noexcept = default;
  record& operator=(record&&) noexcept = default;
  ~record() = default;

  bool operator==(const record& rhs) const;
  bool operator!=(const record& rhs) const;

  template<class Operator>
  void process(Operator &op) {
    for(auto &f : fields_) {
      f.process(op);
    }
  }

  void append(field col);

  [[nodiscard]] const std::vector<field>& columns() const;

  [[nodiscard]] const field& at(const std::string &name) const;
  [[nodiscard]] const field& at(size_t index) const;
  [[nodiscard]] field& at(const std::string &name);
  [[nodiscard]] field& at(size_t index);
  template<class Type>
  std::optional<Type> at(const std::string &name) const {
    return at(name).as<Type>();
  }
  template<class Type>
  std::optional<Type> at(const size_t index) const {
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

private:
  std::vector<field> fields_;
  std::unordered_map<std::string, std::size_t> index_by_name_;
};
}

namespace matador {
using record = query::record;
}

#endif //QUERY_RECORD_HPP
