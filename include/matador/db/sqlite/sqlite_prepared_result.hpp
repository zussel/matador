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
  void serialize(const char *id, char &x) override;
  void serialize(const char *id, short &x) override;
  void serialize(const char *id, int &x) override;
  void serialize(const char *id, long &x) override;
  void serialize(const char *id, long long &x) override;
  void serialize(const char *id, unsigned char &x) override;
  void serialize(const char *id, unsigned short &x) override;
  void serialize(const char *id, unsigned int &x) override;
  void serialize(const char *id, unsigned long &x) override;
  void serialize(const char *id, unsigned long long &x) override;
  void serialize(const char *id, float &x) override;
  void serialize(const char *id, double &x) override;
  void serialize(const char *id, bool &x) override;
  void serialize(const char *id, char *x, size_t s) override;
  void serialize(const char *id, std::string &x, size_t s) override;
  void serialize(const char *id, std::string &x) override;
  void serialize(const char *id, matador::date &x) override;
  void serialize(const char *id, matador::time &x) override;
  void serialize(const char *id, basic_identifier &x) override;
  void serialize(const char *id, identifiable_holder&x, cascade_type) override;

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
