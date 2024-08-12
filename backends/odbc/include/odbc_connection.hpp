#ifndef QUERY_ODBC_CONNECTION_HPP
#define QUERY_ODBC_CONNECTION_HPP

#ifdef _MSC_VER
#ifdef matador_odbc_EXPORTS
#define MATADOR_ODBC_API __declspec(dllexport)
#else
#define MATADOR_ODBC_API __declspec(dllimport)
#endif
#pragma warning(disable: 4355)
#else
#define MATADOR_ODBC_API
#endif

#include "matador/sql/connection_impl.hpp"

#include "odbc_result_reader.hpp"

#include <sqltypes.h>

namespace matador::backends::odbc {

class odbc_connection : public matador::sql::connection_impl
{
public:
  explicit odbc_connection(const sql::connection_info &info);
  void open() override;
  void close() override;
  [[nodiscard]] bool is_open() const override;
  [[nodiscard]] bool is_valid() const override;

  std::unique_ptr<sql::query_result_impl> fetch(const std::string &stmt) override;
  std::unique_ptr<sql::statement_impl> prepare(sql::query_context query) override;

  size_t execute(const std::string &stmt) override;

  std::vector<sql::column_definition> describe(const std::string& table) override;

  bool exists(const std::string &schema_name, const std::string &table_name) override;

private:
  struct fetch_context
  {
    std::vector<sql::column_definition> prototype;
    odbc_result_reader::rows rows;
  };

private:
  static int parse_result(void* param, int column_count, char** values, char** columns);

  fetch_context fetch_internal(const std::string &stmt);

private:
  SQLHANDLE odbc_{};
  SQLHANDLE connection_{};
};

}

extern "C"
{
MATADOR_ODBC_API matador::sql::connection_impl* create_database(const matador::sql::connection_info &info);

MATADOR_ODBC_API void destroy_database(matador::sql::connection_impl *db);
}

#endif //QUERY_ODBC_CONNECTION_HPP
