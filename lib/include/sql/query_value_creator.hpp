#ifndef OOS_QUERY_VALUE_CREATOR_HPP
#define OOS_QUERY_VALUE_CREATOR_HPP

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

#include "sql/value.hpp"

#include "tools/any.hpp"
#include "tools/any_visitor.hpp"

#include <memory>

namespace oos {
namespace detail {

/// @cond OOS_DEV

class OOS_API query_value_creator
{
public:
  query_value_creator();

  std::shared_ptr<oos::detail::basic_value> create_from_any(any &a);

private:
  template < class T >
  void process(T &val)
  {
    value_ = std::make_shared<value<T>>(val);
  }
  void process(char *val);
  void process(const char *val);

private:
  any_visitor visitor_;
  std::shared_ptr<oos::detail::basic_value> value_;
};

/// @endcond

}
}
#endif //OOS_QUERY_VALUE_CREATOR_HPP
