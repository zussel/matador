#include "matador/query/error_code.hpp"

namespace matador::query {

const char * query_category_impl::name() const noexcept {
  return "query";
}

std::string query_category_impl::message(const int ev) const {
  switch (static_cast<error_code>(ev)) {
    case error_code::OK:
      return "OK";
    case error_code::NodeNotFound:
      return "Node not found";
    case error_code::NodeAlreadyExists:
      return "Node already exists";
    case error_code::NoPrimaryKey:
      return "No primary key";
    case error_code::IdentifierTypeMismatch:
      return "Identifier type mismatch";
    case error_code::InvalidArgument:
      return "Invalid argument";
    case error_code::Failure:
      return "Failure";
    default:
      return "Unknown error";
  }
}

const std::error_category & query_category() {
  static query_category_impl instance;
  return instance;
}

std::error_code make_error_code(error_code e) {
  return {static_cast<int>(e), query_category()};
}

std::error_condition make_error_condition(error_code e) {
  return {static_cast<int>(e), query_category()};
}

}