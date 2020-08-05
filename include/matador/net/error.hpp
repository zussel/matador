#ifndef MATADOR_ERROR_HPP
#define MATADOR_ERROR_HPP

#ifdef _MSC_VER
#ifdef matador_net_EXPORTS
#define OOS_NET_API __declspec(dllexport)
#define EXPIMP_NET_TEMPLATE
#else
#define OOS_NET_API __declspec(dllimport)
#define EXPIMP_NET_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_NET_API
#endif

namespace matador {
namespace detail {
OOS_NET_API void throw_logic_error(const char* msg);
OOS_NET_API void throw_logic_error_with_errno(const char* msg, int err);
OOS_NET_API void throw_logic_error_with_gai_errno(const char* msg, int err);
}
}

#endif /* MATADOR_ERROR_HPP */