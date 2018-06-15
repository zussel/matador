//
// Created by sascha on 9/18/15.
//

#ifndef OOS_STATEMENT_IMPL_HPP
#define OOS_STATEMENT_IMPL_HPP

#include "matador/utils/varchar.hpp"

#include "matador/sql/result.hpp"

#ifdef _MSC_VER
#ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_SQL_API
#endif

namespace matador {

class sql;

namespace detail {

/// @cond MATADOR_DEV

class OOS_SQL_API statement_impl : public serializer
{
public:
  ~statement_impl() override = default;

  virtual void clear() = 0;

  virtual detail::result_impl* execute() = 0;

  virtual void reset() = 0;

  template < class T >
  size_t bind(T *o, size_t pos)
  {
    reset();
    host_index = pos;
    matador::access::serialize(static_cast<serializer&>(*this), *o);
    return host_index;
  }

  template < class T >
  size_t bind(T &val, size_t pos)
  {
    host_index = pos;
    serialize("", val);
    return host_index;
  }

  std::string str() const;

protected:
  void str(const std::string &s);

protected:
  size_t host_index;

private:
  std::string sql_;
};

/// @endcond

}
}
#endif //OOS_STATEMENT_IMPL_HPP
