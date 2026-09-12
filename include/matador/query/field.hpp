#ifndef QUERY_FIELD_HPP
#define QUERY_FIELD_HPP

#include "matador/query/column_value.hpp"
#include "matador/query/column_constraint.hpp"

#include <optional>
#include <string>

namespace matador::query {

/**
 * @class Field
 * @brief Represents a generic field that can hold and manage data or behavior.
 *
 * The Field class is a modular component designed to encapsulate a specific unit of information
 * or logic within a larger system. It provides mechanisms to store and manipulate the related
 * data or behavior. This class can be extended or used as a base for customized fields.
 *
 * @details
 * The Field class may include operations such as fetching, modifying, or validating the
 * contained data. It serves as an abstraction for handling information cohesively, ensuring
 * that field-specific logic is encapsulated and reusable. The class may be extended or
 * specialized to fulfill specific application requirements.
 */
class field {
public:
  explicit field(std::string name);
  template<typename Type>
  field(std::string name, Type value, const column_constraints type = column_constraint::None, const size_t size = 0, const int index = -1)
  : name_(std::move(name))
  , type_(type)
  , index_(index)
  , value_(value, size) {}
  field(std::string name, utils::basic_type dt, column_constraints type = column_constraint::None, size_t size = 0, int index = -1);
  field(const field &x) = default;
  field& operator=(const field &x) = default;
  field(field &&x) noexcept;
  field& operator=(field &&x) noexcept;

  bool operator==(const field& rhs) const;
  bool operator!=(const field& rhs) const;

  template<typename Type>
  field& operator=(Type value) {
    value_ = std::move(value);

    return *this;
  }

  [[nodiscard]] const std::string& name() const;
  [[nodiscard]] column_constraints type() const;
  [[nodiscard]] size_t size() const;
  [[nodiscard]] int index() const;
  [[nodiscard]] column_value value() const;

  template<class Type>
  std::optional<Type> as() const {
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

  [[nodiscard]] bool is_primary_key() const;
  [[nodiscard]] bool is_foreign_key() const;
  [[nodiscard]] bool is_attribute() const;

  friend std::ostream& operator<<(std::ostream &out, const field &col);

  template<class Operator>
  void process(Operator &op) {
    op.on_attribute(name_.c_str(), value_, { value_.size(), type_ } );
  }

private:
  friend class record;

  std::string name_;
  column_constraints type_{column_constraint::None};
  int index_{-1};

  column_value value_;
};
}

namespace matador {
using field = query::field;
}

#endif //QUERY_FIELD_HPP
