#ifndef QUERY_CREATE_HPP
#define QUERY_CREATE_HPP

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

#include "object/access.hpp"
#include "object/cascade_type.hpp"
#include "tools/identifier.hpp"

#include "sql/types.hpp"

namespace oos {

class connection;
class sql;
class varchar_base;
class date;
class time;
class basic_object_holder;

/// @cond OOS_DEV

class OOS_API query_create
{
public:
  explicit query_create(sql &d, const connection &db);
  virtual ~query_create();

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
	void serialize(const char *id, basic_object_holder &x, cascade_type);
  template < class T >
  void serialize(const char *id, identifier<T> &x)
  {
    T val = x.value();
    x.serialize(id, val);
    append(" NOT NULL PRIMARY KEY");
  }

private:
  void serialize(const char *id, data_type_t type);
  void serialize(const char *id, data_type_t type, size_t size);

  void append(const std::string &term);

private:  
  sql &dialect;
  const connection &db_;
  bool first;
};

/// @endcond

}

#endif /* QUERY_CREATE_HPP */
