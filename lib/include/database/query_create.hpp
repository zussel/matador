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

#include "database/types.hpp"

namespace oos {

class database;
class sql;

/// @cond OOS_DEV

class OOS_API query_create
{
public:
  explicit query_create(sql &d, const database &db);
  virtual ~query_create();
  
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

private:
  void write(const char *id, data_type_t type);
  void write(const char *id, data_type_t type, size_t size);

private:  
  sql &dialect;
  const database &db_;
  bool first;
};

/// @endcond

}

#endif /* QUERY_CREATE_HPP */
