#ifndef QUERY_CREATE_HPP
#define QUERY_CREATE_HPP

#include "object/object_atomizer.hpp"
#include "database/sql.hpp"

namespace oos {

class database;

class query_create : public object_writer
{
public:
  explicit query_create(sql &d, const database &db);
  virtual ~query_create();
  
  virtual void write(const char *id, char x);
  virtual void write(const char *id, short x);
  virtual void write(const char *id, int x);
  virtual void write(const char *id, long x);
  virtual void write(const char *id, unsigned char x);
  virtual void write(const char *id, unsigned short x);
  virtual void write(const char *id, unsigned int x);
  virtual void write(const char *id, unsigned long x);
  virtual void write(const char *id, float x);
  virtual void write(const char *id, double x);
  virtual void write(const char *id, bool x);
	virtual void write(const char *id, const char *x, int s);
  virtual void write(const char *id, const varchar_base &x);
  virtual void write(const char *id, const std::string &x);
	virtual void write(const char *id, const object_base_ptr &x);
  virtual void write(const char *id, const object_container &x);

private:
  void write(const char *id, sql::data_type_t type);
  void write(const char *id, sql::data_type_t type, int size);

private:  
  sql &dialect;
  const database &db_;
  bool first;
};

}

#endif /* QUERY_CREATE_HPP */
