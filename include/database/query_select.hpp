#ifndef QUERY_SELECT_HPP
#define QUERY_SELECT_HPP

#include "object/object_atomizer.hpp"
#include "database/sql.hpp"

namespace oos {

/// @cond OOS_DEV

class query_select : public object_writer
{
public:
  explicit query_select(sql &s);
  virtual ~query_select();
  
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

  void write(const char *id, data_type_t type);

private:
  sql &dialect;
  bool first;
};

/// @endcond

}

#endif /* QUERY_SELECT_HPP */
