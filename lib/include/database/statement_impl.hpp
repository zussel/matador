//
// Created by sascha on 9/18/15.
//

#include "object/object_atomizer.hpp"

#include "database/result.hpp"

#ifndef OOS_STATEMENT_IMPL_HPP
#define OOS_STATEMENT_IMPL_HPP

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

class serializable;
class sql;

namespace detail {

/// @cond OOS_DEV

class OOS_API statement_impl : public object_writer
{
public:
  virtual ~statement_impl();

  virtual void clear() = 0;

  virtual detail::result_impl* execute() = 0;

  virtual void reset() = 0;

  int bind(serializable *o);

  template < class T >
  int bind(unsigned long i, const T &val)
  {
    host_index = i;
    write("", val);
    return host_index;
  }

  std::string str() const;

protected:
  void str(const std::string &s);

protected:
  int host_index;

private:
  std::string sql_;
};

/// @endcond

}
}
#endif //OOS_STATEMENT_IMPL_HPP
