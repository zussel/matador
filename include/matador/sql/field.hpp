#ifndef QUERY_FIELD_HPP
#define QUERY_FIELD_HPP

#include "matador/utils/value.hpp"

#include "matador/utils/data_types.hpp"

#include <optional>
#include <string>

namespace matador::sql {

class field
{
public:
  explicit field(std::string name);
  template<typename Type>
  field(std::string name, Type value, size_t size = 0, int index = -1)
  : name_(std::move(name))
  , index_(index)
  , value_(value, size) {}
  field(std::string name, data_type dt, size_t size = 0, int index = -1);
  field(const field &x) = default;
  field& operator=(const field &x) = default;
  field(field &&x) noexcept;
  field& operator=(field &&x) noexcept;

  template<typename Type>
  field& operator=(Type value) {
    value_ = std::move(value);

    return *this;
  }

  [[nodiscard]] const std::string& name() const;
  [[nodiscard]] size_t size() const;
  [[nodiscard]] int index() const;

  template<class Type>
  std::optional<Type> as() const
  {
    return value_.as<Type>();
  }

  [[nodiscard]] std::string str() const;

  [[nodiscard]] bool is_integer() const;
  [[nodiscard]] bool is_floating_point() const;
  [[nodiscard]] bool is_bool() const;
  [[nodiscard]] bool is_string() const;
  [[nodiscard]] bool is_varchar() const;
  [[nodiscard]] bool is_blob() const;
  [[nodiscard]] bool is_null() const;
  [[nodiscard]] bool is_unknown() const;

  friend std::ostream& operator<<(std::ostream &out, const field &col);

private:
  template<class Operator>
  void process(Operator &op)
  {
    op.on_attribute(name_.c_str(), value_, value_.size());
  }

private:
  friend class record;

  std::string name_;
  int index_{-1};

  utils::value value_;
};

}
#endif //QUERY_FIELD_HPP
