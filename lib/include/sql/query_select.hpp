#ifndef QUERY_SELECT_HPP
#define QUERY_SELECT_HPP

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

#include "tools/access.hpp"
#include "tools/cascade_type.hpp"

#include "sql/types.hpp"

namespace oos {

/// @cond OOS_DEV

class sql;
class connection;
class varchar_base;
class date;
class time;
class object_holder;

class OOS_API query_select
{
public:
  explicit query_select(sql &s);
  virtual ~query_select();

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
	void serialize(const char *id, identifiable_holder &x, cascade_type);
	template < class T >
	void serialize(const char *id, identifier<T> &x)
	{
		T val = x.value();
		serialize(id, val);
	}

  void serialize(const char *id, data_type_t type);

private:
  sql &dialect;
  bool first;
};

/// @endcond

}

#endif /* QUERY_SELECT_HPP */
