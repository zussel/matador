#ifndef MATADOR_COLUMN_OPTIONS_HPP
#define MATADOR_COLUMN_OPTIONS_HPP

#include "matador_export.h"

#include "matador/query/column_constraint.hpp"

namespace matador::query {
/**
 * This class represents field attributes in
 * the form of size and constraints for a database
 * field (column)
 *
 * Currently, the size is only applied
 * to a field of type string leading
 * to VARCHAR(size).
 */
class MATADOR_EXPORT column_options {
public:
    /**
     * Creates column_options instance
     * with size 0 (zero) and no constraints.
     */
    column_options() = default;
    /**
     * Creates column_options instance
     * with given size and no constraints.
     *
     * @param size Size of the attribute
     */
    column_options(size_t size); // NOLINT(*-explicit-constructor)
    /**
     * Creates column_options instance
     * with size 0 (zero) and given constraints.
     *
     * @param constraints Constraints to apply to field
     */
    column_options(column_constraints constraints); // NOLINT(*-explicit-constructor)
    /**
     * Creates column_options instance
     * with given size and constraints.
     *
     * @param size Size of the attribute
     * @param constraints Constraints to apply to field
     */
    column_options(size_t size, column_constraints constraints);
    ~column_options() = default;

    column_options(const column_options &) = default;
    column_options(column_options &&) = default;
    column_options &operator=(const column_options &) = default;
    column_options &operator=(column_options &&) = default;

    column_options& operator=(size_t size);
    column_options& operator=(column_constraints constraints);
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
    [[nodiscard]] column_constraints constraints() const;

private:
    size_t size_ = 0;
    column_constraints constraints_ = column_constraint::None;
};
}
#endif //MATADOR_COLUMN_OPTIONS_HPP
