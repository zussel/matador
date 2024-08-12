#include "matador/utils/library.hpp"
#include "matador/utils/os.hpp"

#include <utility>

namespace matador::utils {

library::library(std::string lib)
  : lib_(std::move(lib))
{}

library::~library()
{
  unload();
}

bool library::load()
{
    auto path = os::getenv("MATADOR_BACKENDS_PATH");
#if defined(_MSC_VER) || defined(__MINGW32__)
    auto cookie = AddDllDirectory(std::wstring(path.begin(), path.end()).c_str());
    handle_ = LoadLibraryExA((lib_ + ".dll").c_str(), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    RemoveDllDirectory(cookie);
#elif defined(__APPLE__)
    handle_ = dlopen(std::string("lib" + lib_ + ".dylib").c_str(), RTLD_LAZY);
#else
  handle_ = dlopen((path + "/lib" + lib_ + ".so").c_str(), RTLD_LAZY);
#endif
    if (!handle_) {
#if defined(_MSC_VER) || defined(__MINGW32__)
        DWORD errorMessageID = ::GetLastError();
        auto errstr = utils::os::error_string(errorMessageID);
#else
        // TODO: handle win32 and linux error
    fprintf(stdout, "dlopen error: %s", dlerror());
#endif
        return false;
    }
    return true;
}

bool library::load(const std::string &lib)
{
  if (handle_) {
    return false;
  }
  lib_ = lib;
  return load();
}

bool library::unload()
{
  bool ret(true);
  if (handle_) {
#if defined(_MSC_VER) || defined(__MINGW32__)
    ret = FreeLibrary(handle_) == TRUE;
#else
    ret = dlclose(handle_) == 0;
#endif
    if (!ret) {
    } else {
      handle_ = nullptr;
    }
  }
  return ret;
}

func_ptr library::function(const std::string &f) const
{
#if defined(_MSC_VER) || defined(__MINGW32__)
  return GetProcAddress(handle_, f.c_str());
#else
  return dlsym(handle_, f.c_str());
#endif
}

}
