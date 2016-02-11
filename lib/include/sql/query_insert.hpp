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

#include "tools/identifier.hpp"

#include "tools/cascade_type.hpp"

#include "sql/sql.hpp"

#include <sstream>
#include <tools/access.hpp>

namespace oos {

/// @cond OOS_DEV

class connection;
class sql;
class varchar_base;
class date;
class time;
class object_holder;

class OOS_API query_insert : public serializer
{
public:
  explicit query_insert(sql &s);
  virtual ~query_insert();

  template < class T >
  void serialize(T &x)
  {
    oos::access::serialize(*this, x);
  }
  void serialize(const char *id, char &x);
  void serialize(const char *id, short &x);
  void serialize(const char *id, int &x);
  void serialize(const char *id, long &x);
  void serialize(const char *id, unsigned char &x);
  void serialize(const char *id, unsigned short &x);
  void serialize(const char *id, unsigned int &x);
  void serialize(const char *id, unsigned long &x);
  void serialize(const char *id, float &x);
  void serialize(const char *id, double &x);
  void serialize(const char *id, bool &x);
  void serialize(const char *id, char *x, size_t s);
  void serialize(const char *id, varchar_base &x);
  void serialize(const char *id, std::string &x);
  void serialize(const char *id, date &x);
  void serialize(const char *id, time &x);
  void serialize(const char *id, basic_identifier &x);
  void serialize(const char *id, object_holder &x, cascade_type);

  template < class T >
  void write_field(const char *id, data_type_t type, T &x)
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
  void write_field(const char *id, data_type_t type, char &x);
  void write_field(const char *id, data_type_t type, oos::date &x);
  void write_field(const char *id, data_type_t type, oos::time &x);
  void write_field(const char *id, data_type_t type, std::string &x);
  void write_field(const char *id, data_type_t type, varchar_base &x);
  void write_field(const char *id, data_type_t type, char *x, size_t);

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
