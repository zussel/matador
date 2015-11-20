#ifndef SQLITE_PREPARED_RESULT_HPP
#define SQLITE_PREPARED_RESULT_HPP

#include "database/result_impl.hpp"

#include "object/serializable.hpp"

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
  sqlite_prepared_result(sqlite3_stmt *stmt, int rs, std::shared_ptr<oos::object_base_producer> producer);
  ~sqlite_prepared_result();

  virtual const char *column(size_type c) const override;
  virtual bool fetch() override;
  virtual bool fetch(serializable *) override;
  size_type affected_rows() const override;
  size_type result_rows() const override;
  size_type fields() const override;

  virtual int transform_index(int index) const override;

protected:
  virtual void read(const char *id, char &x) override;
  virtual void read(const char *id, short &x) override;
  virtual void read(const char *id, int &x) override;
  virtual void read(const char *id, long &x) override;
  virtual void read(const char *id, unsigned char &x) override;
  virtual void read(const char *id, unsigned short &x) override;
  virtual void read(const char *id, unsigned int &x) override;
  virtual void read(const char *id, unsigned long &x) override;
  virtual void read(const char *id, bool &x) override;
  virtual void read(const char *id, float &x) override;
  virtual void read(const char *id, double &x) override;
  virtual void read(const char *id, char *x, int s) override;
  virtual void read(const char *id, varchar_base &x) override;
  virtual void read(const char *id, std::string &x) override;
  virtual void read(const char *id, oos::date &x) override;
  virtual void read(const char *id, oos::time &x) override;
  virtual void read(const char *id, object_base_ptr &x);
  virtual void read(const char *id, object_container &x);
  virtual void read(const char *id, basic_identifier &x);

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
