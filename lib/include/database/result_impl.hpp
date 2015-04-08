#ifndef RESULT_IMPL_HPP
#define RESULT_IMPL_HPP

#include <object/object_atomizer.hpp>

namespace oos {
namespace detail {

class result_impl : public generic_object_reader<result_impl>
{
protected:
  result_impl();

public:
  virtual ~result_impl();

};

}
}

#endif /* RESULT_IMPL_HPP */
