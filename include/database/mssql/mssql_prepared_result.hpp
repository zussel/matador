#ifndef MSSQL_PREPARED_RESULT_HPP
#define MSSQL_PREPARED_RESULT_HPP

#include "database/result.hpp"

#ifdef WIN32

#else

#endif

#include <vector>
#include <iostream>

namespace oos {

class object_base_ptr;
class varchar_base;

namespace mssql {

class mssql_prepared_result : public result
{
private:
  mssql_prepared_result(const mssql_prepared_result&);
  mssql_prepared_result& operator=(const mssql_prepared_result&);

public:
  typedef result::size_type size_type;

public:
  mssql_prepared_result(int rs);
  ~mssql_prepared_result();
  
  const char* column(size_type c) const;
  bool fetch();
  size_type affected_rows() const;
  size_type result_rows() const;
  size_type fields() const;

  friend std::ostream& operator<<(std::ostream &out, const mssql_prepared_result &res);

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
  size_type affected_rows_;
  size_type rows;
  size_type fields_;
  int result_size;
};

std::ostream& operator<<(std::ostream &out, const mssql_prepared_result &res);

}

}

#endif /* MSSQL_PREPARED_RESULT_HPP */
