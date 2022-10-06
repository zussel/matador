#ifndef MATADOR_STATIC_FILE_SERVICE_HPP
#define MATADOR_STATIC_FILE_SERVICE_HPP

#ifdef _MSC_VER
#ifdef matador_http_EXPORTS
    #define OOS_HTTP_API __declspec(dllexport)
    #define EXPIMP_HTTP_TEMPLATE
  #else
    #define OOS_HTTP_API __declspec(dllimport)
    #define EXPIMP_HTTP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_HTTP_API
#endif

#include "matador/http/http_server.hpp"

#include "matador/logger/log_manager.hpp"

namespace matador {
namespace http {

/**
 * Shortcut function for adding a static file route
 * to the given server. All files at the given path will
 * be served by the given server.
 *
 * @param path Where the static files resides
 * @param s The http server
 */
OOS_HTTP_API void serve_static_files_at(const std::string &path, server &s);

/**
 * This class stands as a static file service
 * for a given server and path.
 */
class OOS_HTTP_API static_file_service
{
public:
  /**
   * Constructor creating a static file route
   * to the given server. All files at the given path will
   * be served by the given server.
   *
   * @param path
   * @param s
   */
  static_file_service(const std::string &path, server &s);

private:
  response serve(const request &req);

private:
  matador::logger log_;
};

}
}

#endif //MATADOR_STATIC_FILE_SERVICE_HPP
