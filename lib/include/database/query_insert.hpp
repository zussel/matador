#ifndef QUERY_INSERT_HPP
#define QUERY_INSERT_HPP

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

#include "object/serializer.hpp"

#include "database/sql.hpp"

#include <sstream>

namespace oos {

/// @cond OOS_DEV

class OOS_API query_insert : public serializer
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
  virtual void write(const char *id, const char *x, size_t s);
  virtual void write(const char *id, const varchar_base &x);
  virtual void write(const char *id, const std::string &x);
  virtual void write(const char *id, const date &x);
  virtual void write(const char *id, const time &x);
  virtual void write(const char *id, const object_base_ptr &x);
  virtual void write(const char *id, const object_container &x);
  virtual void write(const char *id, const basic_identifier &x);

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
  void write_field(const char *id, data_type_t type, const char &x);
  void write_field(const char *id, data_type_t type, const oos::date &x);
  void write_field(const char *id, data_type_t type, const oos::time &x);
  void write_field(const char *id, data_type_t type, const std::string &x);
  void write_field(const char *id, data_type_t type, const varchar_base &x);
  void write_field(const char *id, data_type_t type, const char *x);

  void fields();
  void values();

private:
  void write_null(const char *id);

private:
  sql &dialect;
  bool first;
  bool fields_;
};

/// @endcond

}

#endif /* QUERY_INSERT_HPP */
