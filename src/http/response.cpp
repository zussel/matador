#include "matador/http/response.hpp"
#include "matador/http/response_header.hpp"

using namespace std;

namespace matador {
namespace http {

std::string response::to_string() const
{
  std::string result(http::to_string(status));
  for(const auto &p : headers) {
    result += p.first + ": " + p.second + "\r\n";
  }

  if (!body.empty()) {
    result += response_header::CONTENT_LENGTH + std::string(": ") + ::to_string(content_type.length) + "\r\n";
    result += response_header::CONTENT_TYPE + std::string(": ") + content_type.type;
  }
//  result += response_header::CONTENT_LANGUAGE + std::string(": ") + content_type.language + "\r\n\r\n";

  result += "\r\n\r\n";
  result += body;

  return result;
}


response response::create(http::status_t status)
{
  response resp;
  resp.status = status;

  resp.version.major = 1;
  resp.version.minor = 1;

  resp.headers.insert(std::make_pair(response_header::DATE, matador::to_string(time::now(), "%a, %d %b %Y %H:%M:%S %Z")));
  resp.headers.insert(std::make_pair(response_header::SERVER, "Matador/0.7.0"));
  resp.headers.insert(std::make_pair(response_header::CONNECTION, "Closed"));

  return resp;
}

response response::no_content()
{
  return create(http::NO_CONTENT);
}

response response::not_found()
{
  response resp = create(http::NOT_FOUND);

  return resp;
}

}
}