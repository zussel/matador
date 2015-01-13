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

#ifdef _MSC_VER
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

#ifdef _MSC_VER
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace oos {

#ifndef OOS_DOXYGEN_DOC
#ifdef _MSC_VER
typedef FARPROC func_ptr;
#else
typedef void* func_ptr;
#endif
#endif /* OOS_DOXYGEN_DOC */

/**
 * @class library
 * @brief Helps to load and unload
 *        external libraries.
 *
 * This class represents a loader and unloader
 * for an exernal library. The path to the library
 * is given by a string.
 * It can be loaded and unloaded. Furthermore it
 * provides a method to get a pointer to an function
 * exported by the library. The function is identfied
 * by a name.
 */
class OOS_API library
{
public:
  /**
   * Create a unspecified library object
   */
  library();
  /**
   * Create a library for the given lib path
   *
   * @param lib The path to the library to map.
   */
  explicit library(const std::string &lib);
  ~library();

  /**
   * Load the underlaying library.
   * If the library path is invalid
   * or the library cannot be loaded
   * false is returned.
   *
   * @return True on successful library loading.
   */
  bool load();

  /**
   * Sets the libray (and subsequently close and
   * overwrite an already instatiated library) and
   * load the underlaying library. If the library
   * path is invalid or the library cannot be loaded
   * false is returned.
   *
   * @param lib The path to the library to load.
   * @return True on successful library loading.
   */
  bool load(const std::string &lib);

  /**
   * Unload an loaded library. If
   * the library isn't loaded or an
   * error occured while unloading
   * false is returned.
   *
   * @return True on successful unloading.
   */
  bool unload();

  /**
   * Returns the pointer to the exported
   * function of the library identified
   * by the given name.
   *
   * @param f The name of the exported function.
   * @return The pointer to the function.
   */
  func_ptr function(const std::string &f) const;

private:
  std::string lib_;

#ifdef _MSC_VER
  HMODULE handle_;
#else
  void *handle_;
#endif
};

}

#endif /* LIBRARY_HPP */
