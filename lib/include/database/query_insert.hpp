#ifndef QUERY_INSERT_HPP
#define QUERY_INSERT_HPP

#include "object/object_atomizer.hpp"
#include "database/sql.hpp"

#include <sstream>

namespace oos {

/// @cond OOS_DEV

class query_insert : public object_writer
{
public:
  explicit query_insert(sql &s);
  virtual ~query_insert();
  
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
  virtual void write(const char *id, const primary_key_base &x);

  template < class T >
  void write_field(const char *id, data_type_t type, const T &x)
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
  void write_field(const char *id, data_type_t type, const std::string &x);
  void write_field(const char *id, data_type_t type, const varchar_base &x);
  void write_field(const char *id, data_type_t type, const char *x);
  
  void fields();
  void values();

private:
  sql &dialect;
  bool first;
  bool fields_;
};

/// @endcond

}

#endif /* QUERY_INSERT_HPP */
