//
// Created by sascha on 9/18/15.
//

#ifndef OOS_STATEMENT_IMPL_HPP
#define OOS_STATEMENT_IMPL_HPP

#include "tools/varchar.hpp"

#include "sql/result.hpp"

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

namespace oos {

class sql;

namespace detail {

/// @cond OOS_DEV

class OOS_API statement_impl : public serializer
{
public:
  virtual ~statement_impl();

  virtual void clear() = 0;

  virtual detail::result_impl* execute() = 0;

  virtual void reset() = 0;

  template < class T >
  size_t bind(T *o, size_t pos)
  {
    reset();
    host_index = pos;
    oos::access::serialize(static_cast<serializer&>(*this), *o);
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
