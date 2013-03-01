#ifndef QUERY_SELECT_HPP
#define QUERY_SELECT_HPP

#include "serializer.hpp"
#include "sql.hpp"

class query_select : public serializer
{
public:
  explicit query_select(sql &s);
  virtual ~query_select();
  
  virtual void write(const char *id, long );
  virtual void write(const char *id, double );
  virtual void write(const char *id, const std::string &);

  void write(const char *id, sql::data_type_t type);

private:
  sql &dialect;
  bool first;
};

#endif /* QUERY_SELECT_HPP */
