#ifndef LIBRARY_HPP
#define LIBRARY_HPP

#include "matador/utils/export.hpp"

#include <string>

#if defined(_MSC_VER) || defined(__MINGW32__)
#define _WINSOCKAPI_
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace matador {

#ifndef MATADOR_DOXYGEN_DOC
#if defined(_MSC_VER) || defined(__MINGW32__)
typedef FARPROC func_ptr;
#else
typedef void* func_ptr;
#endif
#endif /* MATADOR_DOXYGEN_DOC */

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
class OOS_UTILS_API library
{
public:
  /**
   * Create a unspecified library serializable
   */
  library() = default;
  /**
   * Create a library for the given lib path
   *
   * @param lib The path to the library to map.
   */
  explicit library(std::string lib);
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

#if defined(_WIN32) || defined(__MINGW32__)
  HMODULE handle_ = nullptr;
#else
  void *handle_ = nullptr;
#endif
};

}

#endif /* LIBRARY_HPP */
