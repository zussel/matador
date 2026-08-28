#ifndef MATADOR_COLUMN_EXPRESSION_HPP
#define MATADOR_COLUMN_EXPRESSION_HPP

#include "matador/query/expression/table_column_expression.hpp"
#include "matador/query/expression/value_expression.hpp"
#include "matador/query/expression/placeholder_expression.hpp"
#include "matador/query/placeholder.hpp"

#include "matador/utils/types.hpp"

namespace matador::query {
class column_expression {
public:
    column_expression() = default;
    explicit column_expression(column_expression_ptr expr) noexcept;

    template<typename Type, typename = std::enable_if_t<std::is_convertible_v<Type, utils::database_type>>>
    column_expression(Type&& value)
    : column_expression(std::make_unique<value_expression>(std::forward<Type>(value))){
    }
    column_expression(placeholder p);
    column_expression(const column_expression&) = delete;
    column_expression& operator=(const column_expression&) = delete;
    column_expression(column_expression&&) noexcept = default;
    column_expression& operator=(column_expression&&) noexcept = default;

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] const abstract_column_expression* get() const noexcept;
    [[nodiscard]] const column_expression_ptr& ptr() const noexcept;
    column_expression_ptr release() noexcept;

    // Move out for APIs that still take column_expression_ptr
    operator column_expression_ptr() && noexcept;

    // The feature you want: turn an expression into a SELECT-able column with alias
    [[nodiscard]] column as(const std::string& alias) &&;

private:
    column_expression_ptr expression_{};
};

// Helper constructors (optional but convenient)
inline column_expression make_column_expression(const column& col) {
    return column_expression{std::make_unique<table_column_expression>(col)};
}

inline column_expression make_column_expression(utils::database_type v) {
    return column_expression{std::make_unique<value_expression>(std::move(v))};
}

inline column_expression make_column_expression(placeholder /*unused*/) {
    return column_expression{std::make_unique<placeholder_expression>()};
}
}
#endif //MATADOR_COLUMN_EXPRESSION_HPP