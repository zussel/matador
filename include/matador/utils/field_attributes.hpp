#ifndef MATADOR_FIELD_ATTRIBUTES_HPP
#define MATADOR_FIELD_ATTRIBUTES_HPP

#include "matador_export.h"

#include "matador/utils/constraints.hpp"

#include <cstddef>

namespace matador::utils {

/**
 * This class represents field attributes in
 * the form of size and constraints for a database
 * field (column)
 *
 * Currently, the size is only applied
 * to a field of type string leading
 * to VARCHAR(size).
 */
class MATADOR_EXPORT field_attributes
{
public:
  /**
   * Creates field_attributes instance
   * with size 0 (zero) and no constraints.
   */
  field_attributes() = default;
  /**
   * Creates field_attributes instance
   * with given size and no constraints.
   *
   * @param size Size of the attribute
   */
  field_attributes(size_t size); // NOLINT(*-explicit-constructor)
  /**
   * Creates field_attributes instance
   * with size 0 (zero) and given constraints.
   *
   * @param options Constraints to apply to field
   */
  field_attributes(constraints options); // NOLINT(*-explicit-constructor)
  /**
   * Creates field_attributes instance
   * with given size and constraints.
   *
   * @param size Size of the attribute
   * @param options Constraints to apply to field
   */
  field_attributes(size_t size, constraints options);
  ~field_attributes() = default;

  field_attributes(const field_attributes &) = default;
  field_attributes(field_attributes &&) = default;
  field_attributes &operator=(const field_attributes &) = default;
  field_attributes &operator=(field_attributes &&) = default;

  field_attributes& operator=(size_t size);
  field_attributes& operator=(constraints opt);
  /**
   * Returns the size of the field
   *
   * @return Size of the field
   */
  [[nodiscard]] size_t size() const;

  /**
   * Returns the constraints of the field
   *
   * @return Constraints of the field
   */
  [[nodiscard]] constraints options() const;

private:
  size_t size_ = 0;
  constraints options_ = constraints::None;
};
}

namespace matador {
const utils::field_attributes NullAttributes {};
const utils::field_attributes VarChar63 {63};
const utils::field_attributes VarChar127 {127};
const utils::field_attributes VarChar255 {255};
const utils::field_attributes VarChar511 {511};
const utils::field_attributes VarChar1023 {1023};
const utils::field_attributes VarChar2047 {2047};
const utils::field_attributes VarChar4095 {4095};
const utils::field_attributes UniqueVarChar63 {63, utils::constraints::Unique};
const utils::field_attributes UniqueVarChar127 {127, utils::constraints::Unique};
const utils::field_attributes UniqueVarChar255 {255, utils::constraints::Unique};
const utils::field_attributes UniqueVarChar511 {511, utils::constraints::Unique};
const utils::field_attributes UniqueVarChar1023 {1023, utils::constraints::Unique};
const utils::field_attributes UniqueVarChar2047 {2047, utils::constraints::Unique};
const utils::field_attributes UniqueVarChar4095 {4095, utils::constraints::Unique};
const utils::field_attributes PrimaryKeyVarChar63 {63, utils::constraints::PrimaryKey};
const utils::field_attributes PrimaryKeyVarChar127 {127, utils::constraints::PrimaryKey};
const utils::field_attributes PrimaryKeyVarChar255 {255, utils::constraints::PrimaryKey};
const utils::field_attributes PrimaryKeyVarChar511 {511, utils::constraints::PrimaryKey};
const utils::field_attributes PrimaryKeyVarChar1023 {1023, utils::constraints::PrimaryKey};
const utils::field_attributes PrimaryKeyVarChar2047 {2047, utils::constraints::PrimaryKey};
const utils::field_attributes PrimaryKeyVarChar4095 {4095, utils::constraints::PrimaryKey};
}

#endif //MATADOR_FIELD_ATTRIBUTES_HPP
