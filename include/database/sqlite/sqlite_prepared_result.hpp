#ifndef SQLITE_PREPARED_RESULT_HPP
#define SQLITE_PREPARED_RESULT_HPP

#include "database/result.hpp"

#include "object/object_atomizable.hpp"
#include "object/object_atomizer.hpp"

#include <mysql/mysql.h>

#include <vector>
#include <iostream>

struct sqlite3_stmt;

namespace oos {

namespace sqlite {

class sqlite_prepared_result : public result, public object_reader
{
private:
  sqlite_prepared_result(const sqlite_prepared_result&);
  sqlite_prepared_result& operator=(const sqlite_prepared_result&);

public:
  typedef unsigned long size_type;

public:
  sqlite_prepared_result(sqlite3_stmt *stmt, int rs);
  ~sqlite_prepared_result();
  
  void get(object_atomizable *o);
  const char* column(size_type c) const;
  bool fetch();
  size_type affected_rows() const;
  size_type result_rows() const;
  size_type fields() const;

  friend std::ostream& operator<<(std::ostream &out, const sqlite_prepared_result &res);

protected:
  virtual void read(const char *id, long &x);
  virtual void read(const char *id, double &x);
  virtual void read(const char *id, std::string &x);

private:
  int ret_;
  bool first_;
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  sqlite3_stmt *stmt_;
  int result_index;
  int result_size;
};

std::ostream& operator<<(std::ostream &out, const sqlite_prepared_result &res);

}

}

#endif /* SQLITE_PREPARED_RESULT_HPP */
