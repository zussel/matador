#ifndef SQLITE_PREPARED_RESULT_HPP
#define SQLITE_PREPARED_RESULT_HPP

#include "database/result.hpp"

#include "object/object_atomizable.hpp"
#include "object/object_atomizer.hpp"

#include <vector>

struct sqlite3_stmt;

namespace oos {

namespace sqlite {

class sqlite_prepared_result : public result
{
private:
  sqlite_prepared_result(const sqlite_prepared_result&);
  sqlite_prepared_result& operator=(const sqlite_prepared_result&);

public:
  typedef result::size_type size_type;

public:
  sqlite_prepared_result(sqlite3_stmt *stmt, int rs);
  ~sqlite_prepared_result();
  
  const char* column(size_type c) const;
  virtual bool fetch();
  virtual bool fetch(object *);
  size_type affected_rows() const;
  size_type result_rows() const;
  size_type fields() const;

  virtual int transform_index(int index) const;

  friend std::ostream& operator<<(std::ostream &out, const sqlite_prepared_result &res);

protected:
  virtual void read(const char *id, char &x);
  virtual void read(const char *id, short &x);
  virtual void read(const char *id, int &x);
  virtual void read(const char *id, long &x);
  virtual void read(const char *id, unsigned char &x);
  virtual void read(const char *id, unsigned short &x);
  virtual void read(const char *id, unsigned int &x);
  virtual void read(const char *id, unsigned long &x);
  virtual void read(const char *id, bool &x);
  virtual void read(const char *id, float &x);
  virtual void read(const char *id, double &x);
  virtual void read(const char *id, char *x, int s);
  virtual void read(const char *id, varchar_base &x);
  virtual void read(const char *id, std::string &x);
  virtual void read(const char *id, object_base_ptr &x);
  virtual void read(const char *id, object_container &x);

private:
  int ret_;
  bool first_;
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  sqlite3_stmt *stmt_;
  int result_size;
};

std::ostream& operator<<(std::ostream &out, const sqlite_prepared_result &res);

}

}

#endif /* SQLITE_PREPARED_RESULT_HPP */
