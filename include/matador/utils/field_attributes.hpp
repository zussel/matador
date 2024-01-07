#ifndef MATADOR_FIELD_ATTRIBUTES_HPP
#define MATADOR_FIELD_ATTRIBUTES_HPP

#include "matador/utils/contraints.hpp"

namespace matador {

/**
 * This class represents field attributes in
 * form of size and constraints for a database
 * field (column)
 *
 * Currently the size is only applied
 * to a field of type string leading
 * to VARCHAR(size).
 */
class field_attributes
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

  /**
   * Returns the size of the field
   *
   * @return Size of the field
   */
  size_t size() const;

  /**
   * Returns the constraints of the field
   *
   * @return Constraints of the field
   */
  constraints options() const;

private:
  size_t size_ = 0;
  constraints options_ = constraints::NONE;
};

const field_attributes null_attributes {};

}
#endif //MATADOR_FIELD_ATTRIBUTES_HPP
