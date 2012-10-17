#include "tools/convert.hpp"

#include <cstring>

namespace oos {
/*
void
convert(const char *from, bool &to)
{
  char *ptr;
  to = strtoul(from, &ptr, 10) > 0;
  // cout << "SUCCEEDED: const char* > bool\n";
}

void
convert(const char *from, std::string &to)
{
  // cout << "SUCCEEDED: const char* > string\n";
  to.assign(from);
}

void
convert(const oos::varchar_base &from, std::string &to)
{
  // cout << "SUCCEEDED: const char* > string\n";
  to = from.str();
}

void
convert(const std::string &from, char *to, size_t num)
{
  if (num > from.size()) {
#ifdef WIN32
    strncpy_s(to, num, from.c_str(), from.size());
#else
    strncpy(to, from.c_str(), from.size());
#endif
    to[from.size()] = '\0';
    // cout << "SUCCEEDED: string > char*\n";
  } else {
    // cout << "FAILED: string > char*\n";
    throw std::bad_cast();
  }
}

void
convert(const oos::varchar_base &from, char *to, size_t num)
{
  if (num > from.size()) {
#ifdef WIN32
    strncpy_s(to, num, from.c_str(), from.size());
#else
    strncpy(to, from.c_str(), from.size());
#endif
    to[from.size()] = '\0';
    // cout << "SUCCEEDED: varchar > char*\n";
  } else {
    // cout << "FAILED: varchar > char*\n";
    throw std::bad_cast();
  }
}

void
convert(const char &from, char *to, size_t num)
{
  // cout << "SUCCEEDED: char > char*\n";
#ifdef WIN32
  _snprintf_s(to, num, num, "%c", from);
#else
  snprintf(to, num, "%c", from);
#endif
}

void
convert(const std::string &from, oos::varchar_base &to)
{
  // cout << "SUCCEEDED: string > varchar\n";
  to = from;
}

void
convert(const char *from, oos::varchar_base &to)
{
  // cout << "SUCCEEDED: const char* > varchar\n";
  to.assign(from);
}

void
convert(const char *from, oos::varchar_base &to, int)
{
  // cout << "SUCCEEDED: const char* > varchar\n";
  to.assign(from);
}

void
convert(const oos::varchar_base &from, oos::varchar_base &to)
{
  to = from;
}
*/
}
