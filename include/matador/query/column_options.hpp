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
class MATADOR_EXPORT column_options final {
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
    column_options(std::size_t size); // NOLINT(*-explicit-constructor)
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
    column_options(std::size_t size, column_constraints constraints);
    ~column_options() = default;

    column_options(const column_options &) = default;
    column_options(column_options &&) = default;
    column_options &operator=(const column_options &) = default;
    column_options &operator=(column_options &&) = default;

    column_options& operator=(std::size_t size);
    column_options& operator=(column_constraints constraints);
    /**
     * Returns the size of the field
     *
     * @return Size of the field
     */
    [[nodiscard]] std::size_t size() const;

    /**
     * Returns the constraints of the field
     *
     * @return Constraints of the field
     */
    [[nodiscard]] const column_constraints& constraints() const;

private:
    std::size_t size_ = 0;
    column_constraints constraints_ = column_constraint::None;
};
}

namespace matador {
const query::column_options NullAttributes {};
const query::column_options VarChar63 {63};
const query::column_options VarChar127 {127};
const query::column_options VarChar255 {255};
const query::column_options VarChar511 {511};
const query::column_options VarChar1023 {1023};
const query::column_options VarChar2047 {2047};
const query::column_options VarChar4095 {4095};
const query::column_options UniqueVarChar63 {63, query::column_constraint::Unique};
const query::column_options UniqueVarChar127 {127, query::column_constraint::Unique};
const query::column_options UniqueVarChar255 {255, query::column_constraint::Unique};
const query::column_options UniqueVarChar511 {511, query::column_constraint::Unique};
const query::column_options UniqueVarChar1023 {1023, query::column_constraint::Unique};
const query::column_options UniqueVarChar2047 {2047, query::column_constraint::Unique};
const query::column_options UniqueVarChar4095 {4095, query::column_constraint::Unique};
const query::column_options PrimaryKeyVarChar63 {63, query::column_constraint::PrimaryKey};
const query::column_options PrimaryKeyVarChar127 {127, query::column_constraint::PrimaryKey};
const query::column_options PrimaryKeyVarChar255 {255, query::column_constraint::PrimaryKey};
const query::column_options PrimaryKeyVarChar511 {511, query::column_constraint::PrimaryKey};
const query::column_options PrimaryKeyVarChar1023 {1023, query::column_constraint::PrimaryKey};
const query::column_options PrimaryKeyVarChar2047 {2047, query::column_constraint::PrimaryKey};
const query::column_options PrimaryKeyVarChar4095 {4095, query::column_constraint::PrimaryKey};
}

#endif //MATADOR_COLUMN_OPTIONS_HPP
