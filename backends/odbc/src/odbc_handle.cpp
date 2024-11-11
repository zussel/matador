#include "odbc_handle.hpp"

#include <sql.h>
#include <sqlext.h>

namespace matador::backends::odbc {

odbc_handle::odbc_handle( const SQLSMALLINT handle_type )
: handle_type_(handle_type) {}

odbc_handle::~odbc_handle() {
    SQLFreeHandle(handle_type_, handle_);
}

utils::result<void, sql::sql_error> odbc_handle::allocate() {
  return allocate(nullptr);
}

utils::result<void, sql::sql_error> odbc_handle::allocate(const odbc_handle& creator) {
  return allocate(creator.handle());
}

utils::result<void, sql::sql_error> odbc_handle::free() {
  if (handle_ == nullptr) {
    return utils::ok<void>();
  }
  if (const auto ret = SQLFreeHandle(handle_type_, handle_); ret != SQL_SUCCESS) {
      return utils::error(make_error(sql::sql_error_code::FAILURE, ret, handle_type_, handle_));
  }
  handle_ = nullptr;
  return utils::ok<void>();
}

bool odbc_handle::is_valid() const {
  return handle_ != nullptr;
}

SQLHANDLE& odbc_handle::handle() {
    return handle_;
}

const SQLHANDLE& odbc_handle::handle() const {
    return handle_;
}

SQLSMALLINT odbc_handle::handle_type() const {
    return handle_type_;
}

utils::result<void, sql::sql_error> odbc_handle::set_attribute(const SQLINTEGER attr, SQLPOINTER value, const SQLINTEGER string_length ) {
    if (attr > 0 && attr < 100) {
        // statement attribute
        if (handle_type_ != SQL_HANDLE_STMT) {
            return utils::error(make_error(sql::sql_error_code::FAILURE, "handle is not statement handle"));
        }
        if (const auto ret = SQLSetStmtAttr(handle_, attr, value, string_length); ret != SQL_SUCCESS) {
            return utils::error(make_error(sql::sql_error_code::FAILURE, ret, handle_type_, handle_));
        }
    } else if ((attr > 100 && attr < 200) || attr > 1000) {
        // connection attribute
        if (handle_type_ != SQL_HANDLE_DBC) {
            return utils::error(make_error(sql::sql_error_code::FAILURE, "handle is not connection handle"));
        }
        if (const auto ret = SQLSetConnectAttr(handle_, attr, value, string_length); ret != SQL_SUCCESS) {
            return utils::error(make_error(sql::sql_error_code::FAILURE, ret, handle_type_, handle_));
        }
    } else if (attr >= 200 && attr < 300) {
        // env attribute
        if (handle_type_ != SQL_HANDLE_ENV) {
            return utils::error(make_error(sql::sql_error_code::FAILURE, "handle is not environment handle"));
        }
        if (const auto ret = SQLSetEnvAttr(handle_, attr, value, string_length); ret != SQL_SUCCESS) {
            return utils::error(make_error(sql::sql_error_code::FAILURE, ret, handle_type_, handle_));
        }
    } else {
      return utils::error(make_error(sql::sql_error_code::FAILURE, "invalid handle attribute"));
    }

    return utils::ok<void>();
}

utils::result<void, sql::sql_error> odbc_handle::allocate(SQLHANDLE creator) {
  if (const SQLRETURN ret = SQLAllocHandle(handle_type_, creator, &handle_); ret != SQL_SUCCESS) {
    std::ignore = free();
    return utils::error(make_error(sql::sql_error_code::FAILURE, ret, handle_type_, handle_));
  }
  return utils::ok<void>();
}

}