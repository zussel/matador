#ifndef SQLITE_PREPARED_RESULT_HPP
#define SQLITE_PREPARED_RESULT_HPP

#include "sql/result_impl.hpp"

#include <vector>

struct sqlite3_stmt;

namespace oos {

namespace sqlite {

class sqlite_prepared_result : public oos::detail::result_impl
{
private:
  sqlite_prepared_result(const sqlite_prepared_result &) = delete;
  sqlite_prepared_result &operator=(const sqlite_prepared_result &) = delete;

public:
  typedef oos::detail::result_impl::size_type size_type;

public:
  sqlite_prepared_result(sqlite3_stmt *stmt, int rs);
  virtual ~sqlite_prepared_result();

  virtual const char *column(size_type c) const override;
  virtual bool fetch() override;
  size_type affected_rows() const override;
  size_type result_rows() const override;
  size_type fields() const override;

  virtual int transform_index(int index) const override;

protected:
  virtual bool prepare_fetch() override;

  virtual bool finalize_fetch() override;

protected:
  virtual void serialize(const char *id, char &x);
  virtual void serialize(const char *id, short &x);
  virtual void serialize(const char *id, int &x);
  virtual void serialize(const char *id, long &x);
  virtual void serialize(const char *id, unsigned char &x);
  virtual void serialize(const char *id, unsigned short &x);
  virtual void serialize(const char *id, unsigned int &x);
  virtual void serialize(const char *id, unsigned long &x);
  virtual void serialize(const char *id, float &x);
  virtual void serialize(const char *id, double &x);
  virtual void serialize(const char *id, bool &x);
  virtual void serialize(const char *id, char *x, size_t s);
  virtual void serialize(const char *id, varchar_base &x);
  virtual void serialize(const char *id, std::string &x);
  virtual void serialize(const char *id, oos::date &x);
  virtual void serialize(const char *id, oos::time &x);
  virtual void serialize(const char *id, basic_identifier &x);
  virtual void serialize(const char *id, identifiable_holder&x, cascade_type);

private:
  int ret_;
  bool first_;
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  sqlite3_stmt *stmt_;
};

}

}

#endif /* SQLITE_PREPARED_RESULT_HPP */
