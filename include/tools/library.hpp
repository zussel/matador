/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include <string>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace oos {

#ifdef WIN32
typedef FARPROC func_ptr;
#else
typedef void* func_ptr;
#endif

class OOS_API library
{
public:
  library();
  explicit library(const std::string &lib);
  ~library();

  bool load();
  bool load(const std::string &lib);
  bool unload();

  func_ptr function(const std::string &f) const;

private:
  std::string lib_;

#ifdef WIN32
  HMODULE handle_;
#else
  void *handle_;
#endif
};

}

#endif /* LIBRARY_HPP */
