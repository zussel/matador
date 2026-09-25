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
    case error_code::RetrieveDataFailed:
      return "Retrieve data failed";
    case error_code::Failure:
      return "Failure";
    case error_code::UnknownTable:
      return "Unknown table";
    case error_code::UnknownColumn:
      return "Unknown column";
    case error_code::BindFailed:
      return "Bind failed";
    case error_code::ExecuteFailed:
      return "Execute failed";
    case error_code::FetchFailed:
      return "Fetch failed";
    case error_code::PrepareFailed:
      return "Prepare failed";
    case error_code::DescribeFailed:
      return "Describe failed";
    case error_code::TableExistsFailed:
      return "Table exists failed";
    case error_code::SequenceExistsFailed:
      return "Sequence exists failed";
    case error_code::ResetFailed:
      return "Reset failed";
    case error_code::OpenError:
      return "Open failed";
    case error_code::CloseError:
      return "Close failed";
    case error_code::StatementLocked:
      return "Statement locked";
    case error_code::InvalidQuery:
      return "Invalid query";
    case error_code::InvalidSchema:
      return "Invalid schema";
    case error_code::InvalidTable:
      return "Invalid table";
    case error_code::InvalidColumn:
      return "Invalid column";
    case error_code::InvalidConstraint:
      return "Invalid constraint";
    case error_code::InvalidDataType:
      return "Invalid data type";
    case error_code::InvalidObject:
      return "Invalid object";
    case error_code::InvalidValue:
      return "Invalid value";
    case error_code::InvalidRelationType:
      return "Invalid relation type";
    case error_code::MissingPrimaryKey:
      return "Missing primary key";
    case error_code::UnknownType:
        return "Unknown type";
    case error_code::FailedToAcquireConnection:
      return "Failed to acquire connection";
    case error_code::FailedToFindPrimaryKey:
      return "Failed to find primary key";
    case error_code::FailedToBuildQuery:
      return "Failed to build query";
    case error_code::FailedToFindObject:
      return "Failed to find object";
    case error_code::FailedToInsertObject:
      return "Failed to insert object";
    case error_code::FailedToAcquirePool:
      return "Failed to acquire pool";
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