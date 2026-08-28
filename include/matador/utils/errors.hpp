#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <system_error>
#include <cstdint>

namespace matador::utils {

enum class utils_error : uint8_t {
  InvalidVersionString,
};

class utils_category_impl final : public std::error_category
{
public:
  [[nodiscard]] const char* name() const noexcept override;
  [[nodiscard]] std::string message(int ev) const override;
};

const std::error_category& utils_category();
std::error_code make_error_code(utils_error e);
std::error_condition make_error_condition(utils_error e);
}

template <>
struct std::is_error_code_enum<matador::utils::utils_error> : true_type {};

#endif //ERRORS_HPP
