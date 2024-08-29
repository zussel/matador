#ifndef QUERY_ODBC_RESULT_READER_HPP
#define QUERY_ODBC_RESULT_READER_HPP

#include "odbc_error.hpp"

#include "matador/sql/query_result_reader.hpp"

#include <sqltypes.h>
#include <sql.h>

#include <vector>

namespace matador::backends::odbc {

class odbc_result_reader final : public sql::query_result_reader
{
public:
  using columns = std::vector<char*>;
  using rows = std::vector<columns>;

public:
  explicit odbc_result_reader(SQLHANDLE stmt);
  ~odbc_result_reader() override;

  [[nodiscard]] size_t column_count() const override;

  [[nodiscard]] const char* column(size_t index) const override;
  [[nodiscard]] bool fetch() override;
  [[nodiscard]] size_t start_column_index() const override;

  void read_value(const char *id, size_t index, char &value) override;
  void read_value(const char *id, size_t index, short &value) override;
  void read_value(const char *id, size_t index, int &value) override;
  void read_value(const char *id, size_t index, long &value) override;
  void read_value(const char *id, size_t index, long long &value) override;
  void read_value(const char *id, size_t index, unsigned char &value) override;
  void read_value(const char *id, size_t index, unsigned short &value) override;
  void read_value(const char *id, size_t index, unsigned &value) override;
  void read_value(const char *id, size_t index, unsigned long &value) override;
  void read_value(const char *id, size_t index, unsigned long long &value) override;
  void read_value(const char *id, size_t index, bool &value) override;
  void read_value(const char *id, size_t index, float &value) override;
  void read_value(const char *id, size_t index, double &value) override;
  void read_value(const char *id, size_t index, matador::time &value) override;
  void read_value(const char *id, size_t index, matador::date &value) override;
  void read_value(const char *id, size_t index, char *value, size_t s) override;
  void read_value(const char *id, size_t index, std::string &value) override;
  void read_value(const char *id, size_t index, std::string &value, size_t size) override;
  void read_value(const char *id, size_t index, utils::blob &value) override;
  void read_value(const char *id, size_t index, sql::value &val, size_t size) override;

private:
  template < class Type >
  void read_column(const char *id, size_t index, Type &val, size_t size = 0)
  {
    SQLLEN info = 0;
    const auto type = static_cast<SQLSMALLINT>(type2int(object::data_type_traits<Type>::type(size)));
    if (const SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(index), type, &val, sizeof(Type), &info); !SQL_SUCCEEDED(ret)) {
      std::string msg{"error on retrieving value for column "};
      msg += std::string(id) + " (type " + typeid(Type).name() + ")";
      throw_odbc_error(ret, SQL_HANDLE_STMT, stmt_, "odbc", msg);
    }
  }

  static int type2int(data_type type);

private:
  SQLHANDLE stmt_;
  long long row_index_ = -1;
  size_t row_count_{};
  size_t column_count_{};
};

}

#endif //QUERY_ODBC_RESULT_READER_HPP
