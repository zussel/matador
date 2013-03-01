#ifndef QUERY_INSERT_HPP
#define QUERY_INSERT_HPP

#include "serializer.hpp"
#include "sql.hpp"

#include <sstream>

class query_insert : public serializer
{
public:
  explicit query_insert(sql &s);
  virtual ~query_insert();
  
  virtual void write(const char *id, long x);
  virtual void write(const char *id, double x);
  virtual void write(const char *id, const std::string &x);

  template < class T >
  void write_field(const char *id, sql::data_type_t type, const T &x)
  {
    if (first) {
      first = false;
    } else {
      dialect.append(", ");
    }
    if (fields_) {
      dialect.append(id);
    } else {
      std::stringstream valstr;
      valstr << x;
      dialect.append(id, type, valstr.str());
    }
  }
  void write_field(const char *id, sql::data_type_t type, const std::string &x);
  
  void fields();
  void values();

private:
  sql &dialect;
  bool first;
  bool fields_;
};

#endif /* QUERY_INSERT_HPP */
