#ifndef MSSQL_RESULT_HPP
#define MSSQL_RESULT_HPP

#include "matador/db/mssql/mssql_exception.hpp"
#include "matador/db/mssql/mssql_statement.hpp"
#include "matador/db/mssql/mssql_types.hpp"

#include "matador/sql/result_impl.hpp"
#include "matador/sql/types.hpp"

#include <vector>

#include <sqltypes.h>
#include <sql.h>

namespace matador {

class row;

namespace mssql {

class mssql_result : public detail::result_impl
{
public:
  mssql_result(const mssql_result&) = delete;
  mssql_result& operator=(const mssql_result&) = delete;

public:
  explicit mssql_result(SQLHANDLE stmt);
  ~mssql_result() override;

  const char* column(size_type c) const override;
  bool fetch() override;

  size_type affected_rows() const override;
  size_type result_rows() const override;
  size_type fields() const override;

  size_type reset_column_index() const override;

  template < class T >
  T get(size_type index, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr) const
  {
    T val;
    SQLLEN info = 0;
    auto type = (SQLSMALLINT)type2int(matador::data_type_traits<T>::builtin_type());
    SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(index), type, &val, sizeof(T), &info);
    if (!SQL_SUCCEEDED(ret)) {
      throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
    }
    return val;
  }

  template < class T >
  T get(size_type index, typename std::enable_if<std::is_same<std::string, T>::value>::type* = nullptr) const
  {
    char buf[1024];
    SQLLEN info = 0;
    SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(index), SQL_C_CHAR, buf, 1024, &info);
    if (!SQL_SUCCEEDED(ret)) {
      throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql");
    }
    return std::string(buf, info);
  }

protected:
  void read_value(const char *id, size_type index, char &x) override;
  void read_value(const char *id, size_type index, short &x) override;
  void read_value(const char *id, size_type index, int &x) override;
  void read_value(const char *id, size_type index, long &x) override;
  void read_value(const char *id, size_type index, long long &x) override;
  void read_value(const char *id, size_type index, unsigned char &x) override;
  void read_value(const char *id, size_type index, unsigned short &x) override;
  void read_value(const char *id, size_type index, unsigned int &x) override;
  void read_value(const char *id, size_type index, unsigned long &x) override;
  void read_value(const char *id, size_type index, unsigned long long &x) override;
  void read_value(const char *id, size_type index, bool &x) override;
  void read_value(const char *id, size_type index, float &x) override;
  void read_value(const char *id, size_type index, double &x) override;
  void read_value(const char *id, size_type index, matador::date &x) override;
  void read_value(const char *id, size_type index, matador::time &x) override;
  void read_value(const char *id, size_type index, char *x, size_t size) override;
  void read_value(const char *id, size_type index, std::string &x) override;
  void read_value(const char *id, size_type index, std::string &x, size_t size) override;

  template < class T >
  void read_column(const char *id, size_type index, T &val)
  {
    SQLLEN info = 0;
    auto type = (SQLSMALLINT)type2int(data_type_traits<T>::builtin_type());
    SQLRETURN ret = SQLGetData(stmt_, (SQLUSMALLINT)(index), type, &val, sizeof(T), &info);
    if (SQL_SUCCEEDED(ret)) {
      return;
    } else {
      std::stringstream msg;
      msg << "error on retrieving value for column " << id << " (type " << typeid(T).name() << ")";
      throw_database_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", msg.str());
    }
  }
  
  void read_column(const char *, size_type index, char &val);
  void read_column(const char *, size_type index, unsigned char &val);
  void read_column(const char *, size_type index, std::string &val);
  void read_column(const char *, size_type index, std::string &val, long);
  void read_column(const char *, size_type index, matador::date &val);
  void read_column(const char *, size_type index, matador::time &val);


  bool prepare_fetch() override;

  bool finalize_fetch() override;

private:
  size_type affected_rows_ = 0;
  size_type rows = 0;
  size_type fields_ = 0;
  
  enum { NUMERIC_LEN = 21 };

  SQLHANDLE stmt_;
};

}

}

#endif /* MSSQL_RESULT_HPP */
