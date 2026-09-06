#ifndef MATADOR_QUERY_UTILS_HPP
#define MATADOR_QUERY_UTILS_HPP

#include "matador/query/column_value.hpp"

#include <string>

namespace matador::query {
class dialect;
class column;
struct query_context;

void prepare_column(query_context& ctx, const dialect& d, const column& col);
void prepare_column(std::string &out, const dialect& d, const column &col);
[[nodiscard]] std::string prepare_identifier(const dialect& d, const column &col);
[[nodiscard]] std::string prepare_criteria(const dialect& d, const column &col);

[[nodiscard]] std::string to_query_string(const column_value &val, const dialect& d);

/**
 * Prepare string literal
 *
 * @param str String literal to be prepared
 * @param d The SQL dialect to use preparing the literal
 */
[[nodiscard]] std::string prepare_literal(const std::string &str, const dialect& d);

/**
 * Prepare SQL dialect identifier for execution
 * and escape quotes and quote the identifier
 * string
 *
 * @param col The identifier string to be prepared
 * @param d The SQL dialect to use preparing the identifier string
 * @return The prepared string
 */
[[nodiscard]] std::string prepare_identifier_string(const std::string &col, const dialect& d);

/**
 * Escape identifier quotes inside identifiers.
 *
 * @param str Identifier to be escaped
 * @param d The SQL dialect to use for escaping
 */
void escape_quotes_in_identifier(std::string &str, const dialect& d);

/**
 * Escape quotes in string literals
 *
 * @param str String literal to be escaped
 * @param d The SQL dialect to use for escaping
 */
void escape_quotes_in_literals(std::string &str, const dialect& d);

}
#endif //MATADOR_QUERY_UTILS_HPP