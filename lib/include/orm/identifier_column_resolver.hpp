/**
 * @author sascha on 4/7/16.
 */

#ifndef OOS_IDENTIFIER_COLUMN_RESOLVER_HPP
#define OOS_IDENTIFIER_COLUMN_RESOLVER_HPP

#include "tools/access.hpp"
#include "tools/identifier.hpp"

#include "sql/column.hpp"

#include <cstddef>

namespace oos {

namespace detail {

class identifier_column_resolver
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
    oos::access::serialize(*this, x);
  }

  template<class T>
  void serialize(const char *, T &) {}
  void serialize(const char *, char *, size_t) { }

  template < class V >
  void serialize(const char *, identifier<V> &x);

  template < class HAS_ONE >
  void serialize(const char*, HAS_ONE&, cascade_type) { }

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
  oos::access::serialize(*this, *obj);
  return col_;
}

template<class V>
void identifier_column_resolver::serialize(const char *id, identifier<V> &)
{
  col_.name = id;
}

}
}

#endif //OOS_IDENTIFIER_COLUMN_RESOLVER_HPP
