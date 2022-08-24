#ifndef MATADOR_ERROR_HPP
#define MATADOR_ERROR_HPP

#include "matador/net/export.hpp"

namespace matador {
namespace detail {

/// @cond MATADOR_DEV

OOS_NET_API void throw_logic_error(const char* msg);
OOS_NET_API void throw_logic_error_with_errno(const char* msg, int err);
OOS_NET_API void throw_logic_error_with_gai_errno(const char* msg, int err);

/// @endcond

}
}

#endif /* MATADOR_ERROR_HPP */