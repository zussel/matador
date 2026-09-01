#ifndef QUERY_BUILDER_EXCEPTION_HPP
#define QUERY_BUILDER_EXCEPTION_HPP

#include "matador/utils/error.hpp"

#include <cstdint>

namespace matador::utils {
class error_exception final : public std::exception {
public:
  explicit error_exception(class error &&err);
  template<typename ErrorCodeType>
  error_exception(ErrorCodeType ec, std::string &&msg)
  : error_(ec, msg)
  {}

  [[nodiscard]] const class error &error() const;

private:
  class error error_;
};
}

namespace matador {
using utils::error_exception;
}

#endif //QUERY_BUILDER_EXCEPTION_HPP
