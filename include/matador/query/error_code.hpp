#ifndef QUERY_ERROR_CODE_HPP
#define QUERY_ERROR_CODE_HPP

#include <cstdint>
#include <system_error>

namespace matador::query {

enum class error_code : uint8_t {
  OK = 0,
  NodeNotFound,
  NodeAlreadyExists,
  NoPrimaryKey,
  IdentifierTypeMismatch,
  InvalidArgument,
  Failure,
};

class query_category_impl final : public std::error_category {
public:
  [[nodiscard]] const char* name() const noexcept override;
  [[nodiscard]] std::string message(int ev) const override;
};

const std::error_category& query_category();
std::error_code make_error_code(error_code e);
std::error_condition make_error_condition(error_code e);

}

template <>
struct std::is_error_code_enum<matador::query::error_code> : true_type {};

#endif //QUERY_ERROR_CODE_HPP
