#include "matador/logger/log_manager.hpp"

#include "matador/http/request_parser.hpp"
#include "matador/http/request.hpp"
#include "matador/http/request_header.hpp"
#include "matador/http/mime_types.hpp"

#if defined(_WIN32) || defined(_WIN64)
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

using namespace std;

namespace matador {
namespace http {

const std::size_t request_parser::CRLF_LEN = strlen(request_parser::CRLF);
const std::size_t request_parser::HTTP_VERSION_PREFIX_LEN = strlen(request_parser::HTTP_VERSION_PREFIX);

request_parser::return_t request_parser::parse(const std::string &msg, request &req)
{
  logger log(create_logger("RequestParser"));
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
  http_prefix_index_ = 0;
  std::string::size_type pos = -1;
  if (state_ != BODY) {
    for (pos = 0; pos < msg.size(); ++pos) {
      std::string::value_type c = msg.at(pos);
      if (c == '\0') {
        return result;
      }
      bool ret = false;
      bool finished = false;
      switch (state_) {
        case METHOD:
          ret = parse_method(c, req);
          break;
        case URL_PATH:
          ret = parse_url_path(c, req);
          break;
        case URL_QUERY_FIELD:
          ret = parse_url_query_field(c);
          break;
        case URL_QUERY_VALUE:
          ret = parse_url_query_value(c, req.query_params_);
          break;
        case URL_FRAGMENT:
          ret = parse_url_fragment(c, req);
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
  }
  /*
   * if HTTP method is POST or PUT
   * try to read body depending on
   * content length (if available) or
   * available data
   */
  if (state_ == BODY && (req.method() == http::POST || req.method() == http::PUT)) {
    auto length = std::stoul(req.content_.length) - req.body_.size();
    ++pos;
    if (length > 0) {
      //log_default(log_level::LVL_INFO, "RequestParser", "length: %d, pos+1: %d, msg.length: %d", length, pos + 1, msg.size());
      // determine body size of message
      auto remaining_msg_size = msg.size() - pos;
      req.body_.append(msg.substr(pos, remaining_msg_size));
      if (remaining_msg_size == length) {
        process_body(req);
        state_ = METHOD;
        result = FINISH;
      } else if (remaining_msg_size < length) {
        result = PARTIAL;
      } else {
        // should not happen
      }
    } else if (msg.size() > pos) {
      req.body_.assign(msg.substr(pos));
      state_ = METHOD;
      result = FINISH;
    }
//    auto length = std::stoul(req.content_.length);
//    log.info("length: %d, pos+1: %d, msg.length: %d", length, pos+1, msg.size());
//    if (length > 0) {
//      req.body_.assign(msg.substr(++pos, length));
//      if (req.body_.size() < length) {
//        result = PARTIAL;
//      } else {
//        process_body(req);
//        result = FINISH;
//      }
//    } else {
//      req.body_.assign(msg.substr(++pos));
//    }
  }

  if (result == FINISH) {
    state_ = METHOD;
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
    state_ = URL_PATH;
    return true;
  } else {
    return false;
  }
}

bool request_parser::parse_url_path(char c, request &req)
{
  if (c == '?') {
    current_query_field_.clear();
    state_ = URL_QUERY_FIELD;
    return true;
  } else if (is_url_char(c)) {
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

bool request_parser::parse_url_query_field(char c)
{
  if (c == '=') {
    state_ = URL_QUERY_VALUE;
    return true;
  } else if (is_url_char(c) && c != '?') {
    current_query_field_.push_back(c);
    return true;
  } else {
    return false;
  }
}

bool request_parser::parse_url_query_value(char c, t_string_param_map &params)
{
  if (hex_parse_state_ == HEX_FIRST) {
    if (!is_hex_char(c)) {
      return false;
    }
    hex_str_.push_back(c);
    hex_parse_state_ = HEX_SECOND;
    return true;
  } else if (hex_parse_state_ == HEX_SECOND) {
    if (!is_hex_char(c)) {
      return false;
    }
    hex_str_.push_back(c);
    char *err;
    char hex = static_cast<char>(std::strtol(hex_str_.c_str(), &err, 16));
    current_query_value_.push_back(hex);
    hex_parse_state_ = HEX_FINISHED;
    return true;
  } else if (c == '%') {
    hex_str_.clear();
    hex_parse_state_ = HEX_FIRST;
    return true;
  } else if (c == '&') {
    if (hex_parse_state_ != HEX_FINISHED) {
      return false;
    }
    params.insert(std::make_pair(current_query_field_, current_query_value_));
    current_query_field_.clear();
    current_query_value_.clear();
    state_ = URL_QUERY_FIELD;
    return true;
  } else if (c == '#') {
    if (hex_parse_state_ != HEX_FINISHED) {
      return false;
    }
    params.insert(std::make_pair(current_query_field_, current_query_value_));
    current_query_field_.clear();
    current_query_value_.clear();
    state_ = URL_FRAGMENT;
    return true;
  } else if (c == '+') {
    current_query_value_.push_back(' ');
    return true;
  } else if (is_url_char(c) && c != '?') {
    current_query_value_.push_back(c);
    return true;
  } else if (c == ' ') {
    if (hex_parse_state_ != HEX_FINISHED) {
      return false;
    }
    params.insert(std::make_pair(current_query_field_, current_query_value_));
    current_query_field_.clear();
    current_query_value_.clear();
    http_prefix_index_ = 0;
    state_ = VERSION;
    return true;
  } else {
    return false;
  }
}

bool request_parser::parse_url_fragment(char c, request &req)
{
  if (is_url_char(c)) {
    req.fragment_.push_back(c);
    return true;
  } else if (c == ' ') {
    http_prefix_index_ = 0;
    state_ = VERSION;
    return true;
  }
  return false;
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

bool request_parser::parse_header_finish(char c)
{
  auto header_finished = c == '\n';
  if (header_finished) {
    state_ = BODY;
  }
  return header_finished;
}

bool request_parser::is_url_char(char c) const
{
  return isalnum(c) || strchr(URL_SPECIAL_CHAR, c) != nullptr;
}

bool request_parser::is_hex_char(char c) const
{
  return isalnum(c) || strchr(HEX_CHAR, c) != nullptr;
}

void request_parser::insert_header(const std::string &key, const std::string &value, request &req)
{
  req.headers_.insert(std::make_pair(key, value));
  if (strcasecmp(key.c_str(), request_header::CONTENT_TYPE) == 0) {
    req.content_.type = value;
  } else if (strcasecmp(key.c_str(), request_header::CONTENT_LENGTH) == 0) {
    req.content_.length = value;
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

void request_parser::process_body(request &req)
{
  if (req.content_.type == mime_types::APPLICATION_X_WWW_FORM_URLENCODED) {
    // parse body as form data
    // and fill form data map in request
    parse_form_data(req);
  }
}

bool request_parser::parse_form_data(request &req)
{
  state_ = URL_QUERY_FIELD;
  std::string::size_type pos;
  bool ret = false;
  for (pos = 0; pos < req.body_.size(); ++pos) {
    std::string::value_type c = req.body_.at(pos);
    if (c == '\0') {
      break;
    }
    bool finished = false;
    ret = false;
    switch (state_) {
      case URL_QUERY_FIELD:
        ret = parse_url_query_field(c);
        break;
      case URL_QUERY_VALUE:
        ret = parse_url_query_value(c, req.form_data_);
        break;
      case VERSION:
        finished = true;
        break;
      default:
        break;
    }
    if (!ret || finished) {
      break;
    }
  }
  if (ret && !current_query_field_.empty()) {
    req.form_data_.insert(std::make_pair(current_query_field_, current_query_value_));
  }
  current_query_field_.clear();
  current_query_field_.clear();
  return ret;
}

void request_parser::reset()
{
  state_ = METHOD;
  http_prefix_index_ = 0;
  current_key_.clear();
  current_value_.clear();
  current_method_.clear();
  blanks_.clear();
  skip_blanks_ = false;
}

}
}

