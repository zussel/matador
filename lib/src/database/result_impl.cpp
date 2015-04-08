//
// Created by sascha on 08.04.15.
//

#include "database/result_impl.hpp"

namespace oos {
namespace detail {

result_impl::result_impl()
  : generic_object_reader<result_impl>(this)
{
}

result_impl::~result_impl()
{
}

}
}