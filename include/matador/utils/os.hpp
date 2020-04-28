#ifndef MATADOR_OS_HPP
#define MATADOR_OS_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
#define OOS_UTILS_API __declspec(dllexport)
#define EXPIMP_UTILS_TEMPLATE
#else
#define OOS_UTILS_API __declspec(dllimport)
#define EXPIMP_UTILS_TEMPLATE extern
#endif
#pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include <cstdio>
#include <string>

namespace matador {
namespace os {

OOS_UTILS_API FILE* fopen(const std::string &path, const char *modes);
OOS_UTILS_API FILE* fopen(const char *path, const char *modes);

OOS_UTILS_API FILE* freopen(const std::string &path, const char *modes, FILE *stream);
OOS_UTILS_API FILE* freopen(const char *path, const char *modes, FILE *stream);

OOS_UTILS_API bool remove(const std::string &name);
OOS_UTILS_API bool remove(const char *name);

OOS_UTILS_API bool rename(const std::string &old_name, const std::string &new_name);
OOS_UTILS_API bool rename(const char *old_name, const char *new_name);

OOS_UTILS_API bool access(const std::string &path, int mode);
OOS_UTILS_API bool access(const char *path, int mode);

OOS_UTILS_API int dup(FILE *stream);

OOS_UTILS_API bool mkdir(const std::string &dirname);
OOS_UTILS_API bool mkdir(const char *dirname);

OOS_UTILS_API bool rmdir(const std::string &dirname);
OOS_UTILS_API bool rmdir(const char *dirname);

OOS_UTILS_API bool mkpath(const std::string &path);
OOS_UTILS_API bool mkpath(const char *path);

OOS_UTILS_API bool is_readable(const std::string &path);
OOS_UTILS_API bool is_readable(const char *path);
OOS_UTILS_API bool is_writable(const std::string &path);
OOS_UTILS_API bool is_writable(const char *path);
OOS_UTILS_API bool exists(const std::string &path);
OOS_UTILS_API bool exists(const char *path);

OOS_UTILS_API size_t file_size(FILE *stream);

}
}
#endif //MATADOR_OS_HPP
