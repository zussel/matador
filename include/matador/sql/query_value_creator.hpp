#ifndef OOS_QUERY_VALUE_CREATOR_HPP
#define OOS_QUERY_VALUE_CREATOR_HPP

#include "matador/sql/value.hpp"

#include "matador/utils/any.hpp"
#include "matador/utils/any_visitor.hpp"

#include <memory>

namespace matador {
namespace detail {

/// @cond MATADOR_DEV

class query_value_creator
{
public:
  query_value_creator();

  std::shared_ptr<value> create_from_any(any &a);

private:
  template < class T >
  void process(T &val)
  {
    value_ = std::make_shared<value>(val);
  }
  void process(char *val);
  void process(const char *val);

private:
  any_visitor visitor_;
  std::shared_ptr<value> value_;
};

/// @endcond

}
}
#endif //OOS_QUERY_VALUE_CREATOR_HPP
