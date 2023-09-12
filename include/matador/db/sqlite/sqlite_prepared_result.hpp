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

  size_type reset_column_index() const override;

  void close() override;

protected:
  bool prepare_fetch() override;

  bool finalize_fetch() override;

protected:
  void read_value(const char *id, size_type index, char &value) override;
  void read_value(const char *id, size_type index, short &value) override;
  void read_value(const char *id, size_type index, int &value) override;
  void read_value(const char *id, size_type index, long &value) override;
  void read_value(const char *id, size_type index, long long &value) override;
  void read_value(const char *id, size_type index, unsigned char &value) override;
  void read_value(const char *id, size_type index, unsigned short &value) override;
  void read_value(const char *id, size_type index, unsigned int &value) override;
  void read_value(const char *id, size_type index, unsigned long &value) override;
  void read_value(const char *id, size_type index, unsigned long long &value) override;
  void read_value(const char *id, size_type index, bool &value) override;
  void read_value(const char *id, size_type index, float &value) override;
  void read_value(const char *id, size_type index, double &value) override;
  void read_value(const char *id, size_type index, matador::time &value) override;
  void read_value(const char *id, size_type index, matador::date &value) override;
  void read_value(const char *id, size_type index, char *value, size_t size) override;
  void read_value(const char *id, size_type index, std::string &value) override;
  void read_value(const char *id, size_type index, std::string &value, size_t size) override;

private:
  int ret_{};
  bool first_{true};
  size_type rows{0};
  size_type fields_{0};
  sqlite3_stmt *stmt_{nullptr};
};

}

}

#endif /* SQLITE_PREPARED_RESULT_HPP */
