#include "matador/http/response.hpp"
#include "matador/http/response_header.hpp"

using namespace std;

namespace matador {
namespace http {

http::status_t response::status() const
{
  return status_;
}

http::version response::version() const
{
  return version_;
}

const http::content &response::content() const
{
  return content_;
}

const t_string_param_map &response::headers() const
{
  return headers_;
}

const std::string &response::body() const
{
  return body_;
}

std::string response::to_string() const
{
  std::string result = "HTTP/" + std::to_string(version_.major) + "." +
    std::to_string(version_.minor) + " " + std::to_string(status_) + " " + http::to_string(status_) + "\r\n";
  for(const auto &p : headers_) {
    result += p.first + ": " + p.second + "\r\n";
  }

  if (!body_.empty()) {
    result += response_header::CONTENT_LENGTH + std::string(": ") + content_.length + "\r\n";
    result += response_header::CONTENT_TYPE + std::string(": ") + content_.type + "\r\n";
  }
//  result += response_header::CONTENT_LANGUAGE + std::string(": ") + content_type.language + "\r\n\r\n";

  result += "\r\n";
  result += body_;

  return result;
}

const char name_value_separator[] = { ':', ' ' };
const char crlf[] = { '\r', '\n' };

std::list<matador::buffer_view> response::to_buffers() const
{
  std::list<buffer_view> buffers;
  buffers.push_back(http::to_buffer(status_));

  for(const auto &p : headers_) {
    buffers.emplace_back(p.first);
    buffers.emplace_back(matador::buffer_view(name_value_separator, 2));
    buffers.emplace_back(p.second);
    buffers.emplace_back(matador::buffer_view(crlf, 2));
  }

  if (!body_.empty()) {
    buffers.emplace_back(response_header::CONTENT_LENGTH);
    buffers.emplace_back(matador::buffer_view(name_value_separator, 2));
    buffers.emplace_back(content_.length);
    buffers.emplace_back(matador::buffer_view(crlf, 2));
    buffers.emplace_back(response_header::CONTENT_TYPE);
    buffers.emplace_back(matador::buffer_view(name_value_separator, 2));
    buffers.emplace_back(content_.type);
    buffers.emplace_back(matador::buffer_view(crlf, 2));
  }

  buffers.emplace_back(matador::buffer_view(crlf, 2));

  if (!body_.empty()) {
    buffers.emplace_back(body_);
  }

  return buffers;
}

response response::create(http::status_t status)
{
  response resp;
  resp.status_ = status;

  resp.version_.major = 1;
  resp.version_.minor = 1;

  resp.headers_.insert(std::make_pair(response_header::DATE, matador::to_string(time::now(), "%a, %d %b %Y %H:%M:%S")));
//  resp.headers_.insert(std::make_pair(response_header::DATE, matador::to_string(time::now(), "%a, %d %b %Y %H:%M:%S %Z")));
  resp.headers_.insert(std::make_pair(response_header::SERVER, "Matador/0.7.0"));
  resp.headers_.insert(std::make_pair(response_header::CONNECTION, "Closed"));

  return resp;
}

response response::from_file(const std::string &file_path)
{
  response resp;

  // Determine the file extension.
  std::size_t last_slash_pos = file_path.find_last_of('/');
  std::size_t last_dot_pos = file_path.find_last_of('.');
  std::string extension;
  if (last_slash_pos == std::string::npos) {
    last_slash_pos = 0;
  }
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
  {
    extension = file_path.substr(last_dot_pos + 1);
  }

  matador::file f(file_path, "r");
//  matador::file f("." + file_path, "r");

  if (!f.is_open()) {
    return response::not_found();
  }

  // obtain file size:
  fseek (f.stream() , 0 , SEEK_END);
  size_t size = ftell(f.stream());
  rewind (f.stream());

  resp.body_.resize(size);

  auto ret = fread(const_cast<char*>(resp.body_.data()), 1, size, f.stream());

  if (ret == 0) {
      perror("fread");
      f.close();
      return response::bad_request();
  }

  f.close();

  resp.status_ = http::OK;

  resp.content_.type = mime_types::from_file_extension(extension);
  resp.content_.length = std::to_string(size);

  resp.version_.major = 1;
  resp.version_.minor = 1;

  resp.headers_.insert(std::make_pair(response_header::DATE, matador::to_string(matador::time::now(), "%a, %d %b %Y %H:%M:%S")));
//  resp.headers_.insert(std::make_pair(response_header::DATE, matador::to_string(matador::time::now(), "%a, %d %b %Y %H:%M:%S %Z")));
  resp.headers_.insert(std::make_pair(response_header::SERVER, "Matador/0.7.0"));
  resp.headers_.insert(std::make_pair(response_header::CONNECTION, "Closed"));

  return resp;
}

response response::ok(const string &body, mime_types::types type)
{
  response resp = create(http::OK);
  resp.body_ = body;
  resp.content_.length = std::to_string(body.size());
  resp.content_.type = mime_types::from_type(type);
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

response response::redirect(const string &location)
{
  auto resp = create(http::MOVED_TEMPORARILY);
  resp.headers_.insert(std::make_pair("Location", location));
  return resp;
}
}
}