#ifndef QUERY_ODBC_PREPARED_RESULT_READER_HPP
#define QUERY_ODBC_PREPARED_RESULT_READER_HPP

#include "odbc_error.hpp"

#include "matador/sql/query_result_reader.hpp"

#include "matador/object/data_type_traits.hpp"

#include <sqltypes.h>
#include <sql.h>

namespace matador::backends::odbc {

class odbc_prepared_result_reader final : public sql::query_result_reader
{
public:
  explicit odbc_prepared_result_reader(SQLHANDLE stmt);

  [[nodiscard]] size_t column_count() const override;
  [[nodiscard]] const char *column(size_t index) const override;
  bool fetch() override;

  void read_value(const char *id, size_t index, char &value) override;
  void read_value(const char *id, size_t index, short &value) override;
  void read_value(const char *id, size_t index, int &value) override;
  void read_value(const char *id, size_t index, long &value) override;
  void read_value(const char *id, size_t index, long long int &value) override;
  void read_value(const char *id, size_t index, unsigned char &value) override;
  void read_value(const char *id, size_t index, unsigned short &value) override;
  void read_value(const char *id, size_t index, unsigned int &value) override;
  void read_value(const char *id, size_t index, unsigned long &value) override;
  void read_value(const char *id, size_t index, unsigned long long int &value) override;
  void read_value(const char *id, size_t index, bool &value) override;
  void read_value(const char *id, size_t index, float &value) override;
  void read_value(const char *id, size_t index, double &value) override;
  void read_value(const char *id, size_t index, char *value, size_t s) override;
  void read_value(const char *id, size_t index, std::string &value) override;
  void read_value(const char *id, size_t index, std::string &value, size_t s) override;
  void read_value(const char *id, size_t index, matador::time &value) override;
  void read_value(const char *id, size_t index, matador::date &value) override;
  void read_value(const char *id, size_t index, utils::blob &value) override;
  void read_value(const char *id, size_t index, sql::value &val, size_t size) override;

  template < class Type >
  void read_column(const char *id, size_t index, Type &val, size_t size = 0)
  {
    SQLLEN info = 0;
    const auto type = static_cast<SQLSMALLINT>(type2int(object::data_type_traits<Type>::type(size)));
    if (const SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(index), type, &val, sizeof(Type), &info); !SQL_SUCCEEDED(ret)) {
      std::string msg{"error on retrieving value for column "};
      msg += std::string(id) + " (type " + typeid(Type).name() + ")";
      throw_odbc_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", msg);
    }
  }

private:
  static int type2int(data_type type);

private:
  SQLHANDLE stmt_;
};
}
#endif //QUERY_ODBC_PREPARED_RESULT_READER_HPP
