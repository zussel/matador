#include "matador/utils/url.hpp"
#include "matador/utils/os.hpp"

#include "matador/http/request.hpp"
#include "matador/http/response.hpp"
#include "matador/http/static_file_service.hpp"

namespace matador {
namespace http {

void serve_static_files_at(const std::string &path, server &s)
{
  s.on_get(path, [](const request &req) {
    std::string path;
    if (!url::decode(req.url(), path)) {
      return response::bad_request();
    }

    // check if there is '..' in path
    if (path.empty() || path[0] != os::DIR_SEPARATOR || path.find("..") != std::string::npos) {
      return response::bad_request();
    }

    if (path[path.size() - 1] == os::DIR_SEPARATOR) {
      path.append("index.html");
    }

    matador::log(log_level::LVL_DEBUG, "StaticFileService", "serving file %s", path.c_str());

    return response::from_file("." + path);
  });

}

static_file_service::static_file_service(const std::string &path, server &s)
  : log_(matador::create_logger("StaticFileService"))
{
  s.on_get(path, [this](const request &req) { return serve(req); });
}

response static_file_service::serve(const request &req)
{
  std::string path;
  if (!url::decode(req.url(), path)) {
    return response::bad_request();
  }

  // check if there is '..' in path
  if (path.empty() || path[0] != os::DIR_SEPARATOR || path.find("..") != std::string::npos) {
    return response::bad_request();
  }

  if (path[path.size() - 1] == os::DIR_SEPARATOR) {
    path.append("index.html");
  }

  log_.debug("serving file %s", path.c_str());

  return response::from_file("." + path);
}
}
}