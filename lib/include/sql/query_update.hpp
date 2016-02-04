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

#include "tools/identifier.hpp"

#include "sql/sql.hpp"

#include <sstream>

namespace oos {

/// @cond OOS_DEV

class connection;
class sql;
class varchar_base;
class date;
class time;
class basic_object_holder;

class OOS_API query_update
{
public:
  explicit query_update(sql &s);
  virtual ~query_update();

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
  void serialize(const char *id, basic_object_holder &x);
  template < class T >
  void serialize(const char *id, identifier<T> &x)
  {
    T val = x.value();
    serialize(id, val);
  }

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
