#ifndef QUERY_UPDATE_HPP
#define QUERY_UPDATE_HPP

#include "object/object_atomizer.hpp"
#include "database/sql.hpp"

#include <sstream>

namespace oos {

/// @cond OOS_DEV

class query_update : public object_writer
{
public:
  explicit query_update(sql &s);
  virtual ~query_update();
  
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

  template < class T >
  void write_pair(const char *id, data_type_t type, const T &x)
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
  void write_pair(const char *id, data_type_t type, const std::string &x);
  void write_pair(const char *id, data_type_t type, const varchar_base &x);
  void write_pair(const char *id, data_type_t type, const char *x);

private:
  sql &dialect;
  bool first;
};

/// @endcond

}

#endif /* QUERY_UPDATE_HPP */
