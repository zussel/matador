#ifndef QUERY_UPDATE_HPP
#define QUERY_UPDATE_HPP

#include "serializer.hpp"
#include "sql.hpp"

#include <sstream>

class query_update : public serializer
{
public:
  explicit query_update(sql &s);
  virtual ~query_update();
  
  virtual void write(const char *id, long x);
  virtual void write(const char *id, double x);
  virtual void write(const char *id, const std::string &x);

  template < class T >
  void write_pair(const char *id, sql::data_type_t type, const T &x)
  {
    if (first) {
      first = false;
    } else {
      dialect.append(", ");
    }
    dialect.append(std::string(id) + "=");
    std::stringstream valstr;
    valstr << x;
    dialect.append(id, type, valstr.str());
  }
  void write_pair(const char *id, sql::data_type_t type, const std::string &x);

private:
  sql &dialect;
  bool first;
};

#endif /* QUERY_UPDATE_HPP */
