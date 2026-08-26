#ifndef ERROR_HPP
#define ERROR_HPP

#include "matador_export.h"

#include <system_error>
#include <optional>
#include <unordered_map>
#include <ostream>
#include <vector>

namespace matador::utils {

class MATADOR_EXPORT error final {
public:
  error() = default;
  template <class ErrorEnumType, std::enable_if_t<std::is_error_code_enum_v<ErrorEnumType>, int> = 0>
  explicit error(ErrorEnumType err, const std::string& msg = "")
    : error(err, std::vector<error>{}, msg)
  {}
  template <class ErrorEnumType, std::enable_if_t<std::is_error_code_enum_v<ErrorEnumType>, int> = 0>
  error(ErrorEnumType err, std::vector<error> trace, std::string  msg = "")
    : ec_(make_error_code(err))
    , error_message_(std::move(msg))
    , error_trace_(std::move(trace))
  {}
  template <class ErrorEnumType, std::enable_if_t<std::is_error_code_enum_v<ErrorEnumType>, int> = 0>
  error(ErrorEnumType err, error error_trace, const std::string& msg = "")
    : error(err, std::vector{std::move(error_trace)}, msg)
  {}
  template <class ErrorEnumType, std::enable_if_t<std::is_error_code_enum_v<ErrorEnumType>, int> = 0>
  error(ErrorEnumType err, std::unordered_map<std::string, std::string> infos, std::string  msg = "")
    : ec_(make_error_code(err))
    , error_message_(std::move(msg))
    , error_infos_(std::move(infos))
  {}

  error(const error&) = default;
  error(error&&) noexcept = default;
  error& operator=(const error&) = default;
  error& operator=(error&&) noexcept = default;
  ~error() = default;

  friend bool operator==(const error &lhs, const error &rhs);
  friend bool operator==(const error &lhs, const std::error_code &rhs);
  friend bool operator==(const error &lhs, const std::error_condition &rhs);
  friend bool operator!=(const error &lhs, const error &rhs);
  friend bool operator!=(const error &lhs, const std::error_code &rhs);
  friend bool operator!=(const error &lhs, const std::error_condition &rhs);
  friend bool operator<(const error &lhs, const error &rhs);
  friend bool operator<(const error &lhs, const std::error_code &rhs);

  friend std::ostream& operator<<(std::ostream &out, const error &err);

  [[nodiscard]] std::string error_code() const;
  [[nodiscard]] std::string message() const;
  [[nodiscard]] std::string category() const;
  [[nodiscard]] std::error_code ec() const;
  [[nodiscard]] const std::vector<error>& error_trace() const;
  [[nodiscard]] std::optional<std::string> error_info(const std::string &key) const;
  void add_error_info(const std::string &key, std::string value);
  void remove_error_info(const std::string &key);
  [[nodiscard]] std::vector<std::string> error_info_keys() const;

private:
  std::error_code ec_;
  std::string error_message_;
  std::vector<error> error_trace_;
  std::unordered_map<std::string, std::string> error_infos_;
};

}
#endif //ERROR_HPP
