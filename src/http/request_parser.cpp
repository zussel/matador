#include "matador/http/request_parser.hpp"
#include "matador/http/request.hpp"
#include "matador/http/request_header.hpp"

using namespace std;

namespace matador {
namespace http {

request_parser::return_t request_parser::parse(const std::string &msg, request &req)
{
  /*
   * parse first line and extract
   * - http method
   * - url_
   * - http version
   *
   * parse header until double CR/LF
   *
   * check for body data
   */

  return_t result = PARTIAL;
  state_ = METHOD;
  http_prefix_index_ = 0;
  std::string::size_type pos;
  for (pos = 0; pos < msg.size(); ++pos) {
    std::string::value_type c = msg.at(pos);
    bool ret = false;
    bool finished = false;
    switch (state_) {
      case METHOD:
        ret = parse_method(c, req);
        break;
      case URL:
        ret = parse_url(c, req);
        break;
      case VERSION:
        ret = parse_version(c);
        break;
      case MAJOR_VERSION:
        ret = parse_major_version(c, req);
        break;
      case MINOR_VERSION:
        ret = parse_minor_version(c, req);
        break;
      case HEADER:
        ret = parse_header(c);
        break;
      case HEADER_KEY:
        ret = parse_header_key(c);
        break;
      case HEADER_VALUE_BEGIN:
        ret = parse_header_value_begin(c);
        break;
      case HEADER_VALUE:
        ret = parse_header_value(c);
        break;
      case HEADER_NEWLINE:
        ret = parse_header_newline(c, req);
        break;
      case HEADER_FINISH:
        ret = parse_header_finish(c);
        if (ret) {
          finished = true;
        }
        break;
      default:
        break;
    }
    if (!ret) {
      return INVALID;
    } else if (finished) {
      result = FINISH;
      break;
    }
  }
  /*
   * if HTTP method is POST or PUT
   * try to read body depending on
   * content length (if available) or
   * available data
   */
  if (result == FINISH && (req.method() == http::POST || req.method() == http::PUT)) {
    if (req.content_.length > 0) {
      req.body_.assign(msg.substr(++pos, req.content_.length));
    } else {
      req.body_.assign(msg.substr(++pos));
    }
  }

  return result;
}

bool request_parser::parse_method(char c, request &req)
{
  if (isupper(c)) {
    current_method_.push_back(c);
    return true;
  } else if (c == ' ') {
    apply_method(current_method_, req);
    current_method_.clear();
    state_ = URL;
    return true;
  } else {
    return false;
  }
}

bool request_parser::parse_url(char c, request &req)
{
  if (isurlchar(c)) {
    req.url_.push_back(c);
    return true;
  } else if (c == ' ') {
    http_prefix_index_ = 0;
    state_ = VERSION;
    return true;
  } else {
    return false;
  }
}

bool request_parser::parse_version(char c)
{
  if (http_prefix_index_ < HTTP_VERSION_PREFIX_LEN) {
    return c == HTTP_VERSION_PREFIX[http_prefix_index_++];
  } else if (c == '/') {
    state_ = MAJOR_VERSION;
    return true;
  } else {
    return false;
  }
}

bool request_parser::parse_major_version(char c, request &req)
{
  if (isdigit(c)) {
    req.version_.major = c - '0';
    return true;
  } else if (c == '.') {
    state_ = MINOR_VERSION;
    return true;
  } else {
    return false;
  }
}

bool request_parser::parse_minor_version(char c, request &req)
{
  if (isdigit(c)) {
    req.version_.minor = c - '0';
    return true;
  } else if (c == '\r') {
    state_ = HEADER;
    return true;
  } else {
    return false;
  }
}

bool request_parser::parse_header(char c)
{
  if (c == '\n') {
    current_key_.clear();
    current_value_.clear();
    state_ = HEADER_KEY;
    return true;
  } else {
    return false;
  }
}

bool request_parser::parse_header_key(char c)
{
  if (isalnum(c) || c == '-') {
    current_key_.push_back(c);
    return true;
  } else if (c == ':') {
    state_ = HEADER_VALUE_BEGIN;
    return true;
  } else return c == ' ';
}

bool request_parser::parse_header_value_begin(char c)
{
  if (c == ' ') {
    return true;
  } else if (isalnum(c) || ispunct(c)) {
    blanks_.clear();
    current_value_.clear();
    current_value_.push_back(c);
    state_ = HEADER_VALUE;
    return true;
  }
  return false;
}

bool request_parser::parse_header_value(char c)
{
  if (c == ' ' || c == '\t') {
    blanks_.push_back(c);
    return true;
  } else if (isalnum(c) || ispunct(c)) {
    if (!blanks_.empty() && !skip_blanks_) {
      current_value_.append(blanks_);
      blanks_.clear();
    } else if (skip_blanks_) {
      blanks_.clear();
      skip_blanks_ = false;
    }
    current_value_.push_back(c);
    return true;
  } else if (c == '\r') {
    state_ = HEADER_NEWLINE;
    return true;
  }
  return false;
}

bool request_parser::parse_header_newline(char c, request &req)
{
  if (c == '\n') {
    return true;
  } else if (c == ' ' || c == '\t') {
    state_ = HEADER_VALUE;
    skip_blanks_ = true;
    return true;
  } else if (isalnum(c) || ispunct(c)) {
    insert_header(current_key_, current_value_, req);
    current_key_.clear();
    current_key_.push_back(c);
    state_ = HEADER_KEY;
    return true;
  } else if (c == '\r') {
    insert_header(current_key_, current_value_, req);
    current_key_.clear();
    state_ = HEADER_FINISH;
    return true;
  }
  return false;
}

bool request_parser::parse_header_finish(char c) {
  return c == '\n';
}

bool request_parser::isurlchar(char c) const
{
  return isalnum(c) || strchr(URL_SPECIAL_CHAR, c) != 0;
}

void request_parser::insert_header(const std::string &key, const std::string &value, request &req)
{
  req.headers_.insert(std::make_pair(key, value));
  if (strcasecmp(key.c_str(), request_header::CONTENT_TYPE) == 0) {
    req.content_.type = value;
  } else if (strcasecmp(key.c_str(), request_header::CONTENT_LENGTH) == 0) {
    char *end;
    req.content_.length = strtoul(value.c_str(), &end, 10);
  } else if (strcasecmp(key.c_str(), request_header::CONTENT_MD5) == 0) {
    req.content_.md5 = value;
  } else if (strcasecmp(key.c_str(), request_header::HOST) == 0) {
    req.host_ = value;
  }
}

void request_parser::apply_method(const std::string &method, request &req)
{
  req.method_ = http::to_method(method);
}

void request_parser::reset()
{
  state_ = METHOD;
  http_prefix_index_ = 0;
  current_key_.clear();
  current_value_.clear();
  current_method_.clear();
  blanks_.clear();
}

}
}

