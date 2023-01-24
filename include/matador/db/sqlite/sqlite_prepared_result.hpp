#ifndef SQLITE_PREPARED_RESULT_HPP
#define SQLITE_PREPARED_RESULT_HPP

#include "matador/sql/result_impl.hpp"

#include <vector>

struct sqlite3_stmt;

namespace matador {

namespace sqlite {

class sqlite_prepared_result : public matador::detail::result_impl
{
public:
  sqlite_prepared_result(const sqlite_prepared_result &) = delete;
  sqlite_prepared_result &operator=(const sqlite_prepared_result &) = delete;

public:
  typedef matador::detail::result_impl::size_type size_type;

public:
  sqlite_prepared_result(sqlite3_stmt *stmt, int rs);
  ~sqlite_prepared_result() override = default;

  const char *column(size_type c) const override;
  bool fetch() override;
  size_type affected_rows() const override;
  size_type result_rows() const override;
  size_type fields() const override;

  int transform_index(int index) const override;

protected:
  bool prepare_fetch() override;

  bool finalize_fetch() override;

protected:
  void read_value(const char *id, int index, int row, char &value) override;
  void read_value(const char *id, int index, int row, short &value) override;
  void read_value(const char *id, int index, int row, int &value) override;
  void read_value(const char *id, int index, int row, long &value) override;
  void read_value(const char *id, int index, int row, long long &value) override;
  void read_value(const char *id, int index, int row, unsigned char &value) override;
  void read_value(const char *id, int index, int row, unsigned short &value) override;
  void read_value(const char *id, int index, int row, unsigned int &value) override;
  void read_value(const char *id, int index, int row, unsigned long &value) override;
  void read_value(const char *id, int index, int row, unsigned long long &value) override;
  void read_value(const char *id, int index, int row, bool &value) override;
  void read_value(const char *id, int index, int row, float &value) override;
  void read_value(const char *id, int index, int row, double &value) override;
  void read_value(const char *id, int index, int row, matador::time &value) override;
  void read_value(const char *id, int index, int row, matador::date &value) override;
  void read_value(const char *id, int index, int row, char *value, long size) override;
  void read_value(const char *id, int index, int row, std::string &value) override;
  void read_value(const char *id, int index, int row, std::string &value, long size) override;

private:
  int ret_;
  bool first_;
  size_type rows;
  size_type fields_;
  sqlite3_stmt *stmt_;
};

}

}

#endif /* SQLITE_PREPARED_RESULT_HPP */
