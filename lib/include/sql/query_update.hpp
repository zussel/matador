#ifndef QUERY_UPDATE_HPP
#define QUERY_UPDATE_HPP

#ifdef _MSC_VER
#ifdef oos_EXPORTS
#define OOS_API __declspec(dllexport)
#define EXPIMP_TEMPLATE
#else
#define OOS_API __declspec(dllimport)
#define EXPIMP_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_API
#endif

#include "sql/sql.hpp"

#include <sstream>

namespace oos {

/// @cond OOS_DEV

class OOS_API query_update
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
	virtual void write(const char *id, const char *x, size_t s);
  virtual void write(const char *id, const varchar_base &x);
  virtual void write(const char *id, const std::string &x);
	virtual void write(const char *id, const date &x);
	virtual void write(const char *id, const time &x);
	virtual void write(const char *id, const object_base_ptr &x);
  virtual void write(const char *id, const object_container &x);
  virtual void write(const char *id, const basic_identifier &x);

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
  void write_pair(const char *id, data_type_t type, const oos::date &x);
  void write_pair(const char *id, data_type_t type, const oos::time &x);
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
