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
    result += response_header::CONTENT_LENGTH + std::string(": ") + content_type.length + "\r\n";
    result += response_header::CONTENT_TYPE + std::string(": ") + content_type.type;
  }
//  result += response_header::CONTENT_LANGUAGE + std::string(": ") + content_type.language + "\r\n\r\n";

  result += "\r\n\r\n";
  result += body;

  return result;
}

const char name_value_separator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };

std::list<matador::buffer_view> response::to_buffers() const
{
  std::list<buffer_view> buffers;
  buffers.push_back(http::to_buffer(status));

  for(const auto &p : headers) {
    buffers.emplace_back(p.first);
    buffers.emplace_back(matador::buffer_view(name_value_separator, 2));
    buffers.emplace_back(p.second);
    buffers.emplace_back(matador::buffer_view(crlf, 2));
  }

  if (!body.empty()) {
    buffers.emplace_back(response_header::CONTENT_LENGTH);
    buffers.emplace_back(matador::buffer_view(name_value_separator, 2));
    buffers.emplace_back(content_type.length);
    buffers.emplace_back(matador::buffer_view(crlf, 2));
    buffers.emplace_back(response_header::CONTENT_TYPE);
    buffers.emplace_back(matador::buffer_view(name_value_separator, 2));
    buffers.emplace_back(content_type.type);
    buffers.emplace_back(matador::buffer_view(crlf, 2));
  }

  buffers.emplace_back(matador::buffer_view(crlf, 2));

  buffers.emplace_back(body);

  return buffers;
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
  return create(http::NOT_FOUND);
}

response response::bad_request()
{
  return create(http::BAD_REQUEST);
}

}
}