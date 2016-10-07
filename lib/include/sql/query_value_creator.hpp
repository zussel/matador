#ifndef OOS_QUERY_VALUE_CREATOR_HPP
#define OOS_QUERY_VALUE_CREATOR_HPP

#include "sql/value.hpp"

#include "tools/any.hpp"
#include "tools/any_visitor.hpp"

#include <memory>

namespace oos {
namespace detail {

class query_value_creator
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
}
}
#endif //OOS_QUERY_VALUE_CREATOR_HPP
