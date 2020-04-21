#ifndef MATADOR_OS_HPP
#define MATADOR_OS_HPP

#include <cstdio>
#include <string>

namespace matador {
namespace os {

FILE* fopen(const std::string &path, const char *modes);
FILE* fopen(const char *path, const char *modes);

FILE* freopen(const std::string &path, const char *modes, FILE *stream);
FILE* freopen(const char *path, const char *modes, FILE *stream);

bool remove(const std::string &name);
bool remove(const char *name);
bool access(const std::string &path, int mode);
bool access(const char *path, int mode);

int dup(FILE *stream);

bool is_readable(const std::string &path);
bool is_readable(const char *path);
bool is_writable(const std::string &path);
bool is_writable(const char *path);
bool exists(const std::string &path);
bool exists(const char *path);

size_t file_size(FILE *stream);

}
}
#endif //MATADOR_OS_HPP
