#include "matador/utils/error_exception.hpp"

namespace matador::utils {
error_exception::error_exception(class error &&err)
: error_(std::move(err)) {}

const class error& error_exception::error() const {
  return error_;
}

}