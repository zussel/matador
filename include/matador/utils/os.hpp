#ifndef MATADOR_OS_HPP
#define MATADOR_OS_HPP

#include "matador_export.h"

#include <cstdio>
#include <string>

namespace matador::utils::os {
enum class override_env_value : uint8_t {
  KeepValue,
  OverrideValue
};

#ifdef _WIN32
std::string error_string(unsigned long error);
#endif

void setenv(const char *name, const char *value, override_env_value override_value);
std::string getenv(const char* name);
void unsetenv(const char *name);
}

namespace matador::os {

/// @cond MATADOR_DEV

#ifdef _WIN32
MATADOR_EXPORT extern char DIR_SEPARATOR;
MATADOR_EXPORT extern const char* DIR_SEPARATOR_STRING;
#else
MATADOR_EXPORT extern char DIR_SEPARATOR;
MATADOR_EXPORT extern const char* DIR_SEPARATOR_STRING;
#endif

MATADOR_EXPORT FILE* fopen(const std::string &path, const char *modes);
MATADOR_EXPORT FILE* fopen(const char *path, const char *modes);

MATADOR_EXPORT FILE* freopen(const std::string &path, const char *modes, FILE *stream);
MATADOR_EXPORT FILE* freopen(const char *path, const char *modes, FILE *stream);

MATADOR_EXPORT bool fclose(FILE *f);

MATADOR_EXPORT bool remove(const std::string &name);
MATADOR_EXPORT bool remove(const char *name);

MATADOR_EXPORT bool rename(const std::string &old_name, const std::string &new_name);
MATADOR_EXPORT bool rename(const char *old_name, const char *new_name);

MATADOR_EXPORT bool access(const std::string &path, int mode);
MATADOR_EXPORT bool access(const char *path, int mode);

MATADOR_EXPORT int dup(FILE *stream);

MATADOR_EXPORT bool mkdir(const std::string &dirname);
MATADOR_EXPORT bool mkdir(const char *dirname);

MATADOR_EXPORT bool chdir(const std::string &dirname);
MATADOR_EXPORT bool chdir(const char *dirname);

MATADOR_EXPORT bool rmdir(const std::string &dirname);
MATADOR_EXPORT bool rmdir(const char *dirname);

MATADOR_EXPORT std::string get_current_dir();

MATADOR_EXPORT bool mkpath(const std::string &path);
MATADOR_EXPORT bool mkpath(const char *path);

MATADOR_EXPORT bool rmpath(const std::string &path);
MATADOR_EXPORT bool rmpath(const char *path);

MATADOR_EXPORT bool is_readable(const std::string &path);
MATADOR_EXPORT bool is_readable(const char *path);
MATADOR_EXPORT bool is_writable(const std::string &path);
MATADOR_EXPORT bool is_writable(const char *path);
MATADOR_EXPORT bool exists(const std::string &path);
MATADOR_EXPORT bool exists(const char *path);

MATADOR_EXPORT size_t file_size(FILE *stream);

MATADOR_EXPORT std::string build_path(const std::string &a, const std::string &b);

template <typename ...T>
std::string build_path(const std::string &a, const std::string &b, T& ...arg)
{
  return build_path(build_path(a, b), arg...);
}

template<typename... ARGS>
int sprintf(char* str, size_t s, const char* format, ARGS const&... args)
{
#ifdef _WIN32
    return sprintf_s(str, s, format, args...);
#else
    return ::snprintf(str, s, format, args...);
#endif
}

MATADOR_EXPORT char* strerror(int err, char* errbuf, size_t bufsize);

/**
 * Multi platform version of localtime
 *
 * @param in time_t value to be converted
 * @param out converted value
 */
MATADOR_EXPORT void localtime(const time_t &in, struct tm &out);

/// @endcond

}

#endif //MATADOR_OS_HPP
