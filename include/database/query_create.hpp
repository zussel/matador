#ifndef QUERY_CREATE_HPP
#define QUERY_CREATE_HPP

#include "serializer.hpp"
#include "sql.hpp"

class database;

class query_create : public serializer
{
public:
  explicit query_create(sql &d, const database &db);
  virtual ~query_create();
  
  virtual void write(const char *id, long x);
  virtual void write(const char *id, double x);
  virtual void write(const char *id, const std::string &x);
  
private:
  void write(const char *id, sql::data_type_t type);

private:  
  sql &dialect;
  const database &db_;
  bool first;
};

#endif /* QUERY_CREATE_HPP */
