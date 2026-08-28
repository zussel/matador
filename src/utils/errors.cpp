#include "matador/utils/errors.hpp"

namespace matador::utils {

const char * utils_category_impl::name() const noexcept {
  return "utils";
}

std::string utils_category_impl::message(const int ev) const {
  switch (static_cast<utils_error>(ev)) {
    case (utils_error::InvalidVersionString):
      return "Invalid version string";
    default:
      return "Unknown error";
  }
}

const std::error_category & sql_category() {
  static utils_category_impl instance;
  return instance;
}

std::error_code make_error_code(utils_error e) {
  return {static_cast<int>(e), sql_category()};
}

std::error_condition make_error_condition(utils_error e) {
  return {static_cast<int>(e), sql_category()};
}

}
