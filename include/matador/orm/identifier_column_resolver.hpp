/**
 * @author sascha on 4/7/16.
 */

#ifndef OOS_IDENTIFIER_COLUMN_RESOLVER_HPP
#define OOS_IDENTIFIER_COLUMN_RESOLVER_HPP

#ifdef _MSC_VER
#ifdef oos_orm_EXPORTS
#define OOS_ORM_API __declspec(dllexport)
#define EXPIMP_ORM_TEMPLATE
#else
#define OOS_ORM_API __declspec(dllimport)
#define EXPIMP_ORM_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_ORM_API
#endif

#include "matador/utils/access.hpp"
#include "matador/utils/identifier.hpp"

#include "matador/sql/column.hpp"

#include <cstddef>

namespace matador {

namespace detail {

/// @cond OOS_DEV

class OOS_ORM_API identifier_column_resolver
{
public:
  identifier_column_resolver() : col_("") {}

  template < class T >
  static column resolve();

  template < class T >
  column resolve(T *obj);

  template<class T>
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  template<class T>
  void serialize(const char *, T &) {}
  void serialize(const char *, char *, size_t) { }

  template < class V >
  void serialize(const char *, identifier<V> &x);

  template < class HAS_ONE >
  void serialize(const char*, HAS_ONE&, cascade_type) { }

  template < class HAS_MANY >
  void serialize(const char*, HAS_MANY&, const char*, const char*) {}

private:
  column col_;
};

template<class T>
column identifier_column_resolver::resolve()
{
  identifier_column_resolver resolver;
  T obj;
  return resolver.resolve(&obj);
}

template<class T>
column identifier_column_resolver::resolve(T *obj)
{
  matador::access::serialize(*this, *obj);
  return col_;
}

template<class V>
void identifier_column_resolver::serialize(const char *id, identifier<V> &)
{
  col_.name = id;
}

/// @endcond

}
}

#endif //OOS_IDENTIFIER_COLUMN_RESOLVER_HPP
