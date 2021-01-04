#include "matador/http/url.hpp"
#include "matador/http/request.hpp"
#include "matador/http/response.hpp"
#include "matador/http/static_file_service.hpp"

namespace matador {
namespace http {

static_file_service::static_file_service(const std::string &path, server &s)
  : server_(s)
  , log_(matador::create_logger("StaticFileService"))
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
  if (path.empty() || path[0] != '/' || path.find("..") != std::string::npos) {
    return response::bad_request();
  }

  if (path[path.size() - 1] == '/') {
    path.append("index.html");
  }

  // Determine the file extension.
  std::size_t last_slash_pos = path.find_last_of('/');
  std::size_t last_dot_pos = path.find_last_of('.');
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
  {
    extension = path.substr(last_dot_pos + 1);
  }

  matador::file f("." + path, "r");

  if (!f.is_open()) {
    return response::not_found();
  }

  log_.info("serving file %s", path.c_str());

  // obtain file size:
  fseek (f.stream() , 0 , SEEK_END);
  size_t size = ftell (f.stream());
  rewind (f.stream());

  response resp;
  resp.body.resize(size);

  fread(const_cast<char*>(resp.body.data()), 1, size, f.stream());

  f.close();

  resp.status = http::OK;

  resp.content_type.type = mime_types::from_file_extension(extension);
  resp.content_type.length = std::to_string(size);

  resp.version.major = 1;
  resp.version.minor = 1;

  resp.headers.insert(std::make_pair(response_header::DATE, to_string(matador::time::now(), "%a, %d %b %Y %H:%M:%S %Z")));
  resp.headers.insert(std::make_pair(response_header::SERVER, "Matador/0.7.0"));
  resp.headers.insert(std::make_pair(response_header::CONNECTION, "Closed"));

  return resp;
}
}
}