#include "matador/http/response_parser.hpp"
#include "matador/http/response.hpp"

#include <cstring>

#include "matador/logger/log_manager.hpp"

#if defined(_WIN32) || defined(_WIN64)
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

namespace matador {
namespace http {

const std::size_t response_parser::CRLF_LEN = strlen(response_parser::CRLF);
const std::size_t response_parser::HTTP_VERSION_PREFIX_LEN = strlen(response_parser::HTTP_VERSION_PREFIX);

response_parser::return_t response_parser::parse(const std::string &msg, response &resp)
{
  /*
   * parse first line and extract
   * - http version
   * - status code
   * - status text
   *
   * parse header until double CR/LF
   *
   * check for body data
   */
  return_t result = PARTIAL;
  std::string::size_type pos = -1;
  if (state_ != BODY) {
    http_prefix_index_ = 0;
    for (pos = 0; pos < msg.size(); ++pos) {
      std::string::value_type c = msg.at(pos);
      bool ret = false;
      bool finished = false;
      switch (state_) {
        case VERSION:
          ret = parse_version(c);
          break;
        case MAJOR_VERSION:
          ret = parse_major_version(c, resp);
          break;
        case MINOR_VERSION:
          ret = parse_minor_version(c, resp);
          break;
        case STATUS_CODE:
          ret = parse_status_code(c, resp);
          break;
        case STATUS_TEXT:
          ret = parse_status_text(c, resp);
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
          ret = parse_header_newline(c, resp);
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

  if (state_ == BODY) {
    auto length = std::stoul(resp.content_.length) - resp.body_.size();
    ++pos;
    if (length > 0) {
      //log_default(log_level::LVL_INFO, "ResponseParser", "length: %d, pos+1: %d, msg.length: %d", length, pos + 1, msg.size());
      // determine body size of message
      auto remaining_msg_size = msg.size() - pos;
      resp.body_.append(msg.substr(pos, remaining_msg_size));
      if (remaining_msg_size == length) {
        state_ = VERSION;
        result = FINISH;
      } else if (remaining_msg_size < length) {
        result = PARTIAL;
      } else {
        // should not happen
      }
    } else if (msg.size() > pos) {
      resp.body_.assign(msg.substr(pos));
      state_ = VERSION;
      result = FINISH;
    }
  }
  return result;
}

bool response_parser::parse_version(char c)
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

bool response_parser::parse_major_version(char c, response &resp)
{
  if (isdigit(c)) {
    resp.version_.major = c - '0';
    return true;
  } else if (c == '.') {
    state_ = MINOR_VERSION;
    return true;
  } else {
    return false;
  }
}

bool response_parser::parse_minor_version(char c, response &resp)
{
  if (isdigit(c)) {
    resp.version_.minor = c - '0';
    return true;
  } else if (c == ' ') {
    state_ = STATUS_CODE;
    return true;
  } else {
    return false;
  }
}

bool response_parser::parse_status_code(char c, response &resp)
{
  if (isdigit(c)) {
    current_status_.push_back(c);
    return true;
  } else if (c == ' ') {
    resp.status_ = http::to_status(current_status_);
    current_status_.clear();
    state_ = STATUS_TEXT;
    return true;
  } else {
    return false;
  }
}

bool response_parser::parse_status_text(char c, response &)
{
  if (isalnum(c) || c == ' ') {
    current_status_.push_back(c);
    return true;
  } else if (c == '\r') {
    current_status_.clear();
    state_ = HEADER;
    return true;
  } else {
    return false;
  }
}

bool response_parser::parse_header(char c)
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

bool response_parser::parse_header_key(char c)
{
  if (isalnum(c) || c == '-') {
    current_key_.push_back(c);
    return true;
  } else if (c == ':') {
    state_ = HEADER_VALUE_BEGIN;
    return true;
  } else return c == ' ';
}

bool response_parser::parse_header_value_begin(char c)
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

bool isutf8(char c)
{
    static const unsigned char comp = 128;
    return (c & comp) > 0;
}

bool response_parser::parse_header_value(char c)
{
  if (c == ' ' || c == '\t') {
    blanks_.push_back(c);
    return true;
  } else if (c == '\r') {
      state_ = HEADER_NEWLINE;
      return true;
  } else if (isutf8(c) || isalnum(c) || ispunct(c)) {
    if (!blanks_.empty() && !skip_blanks_) {
      current_value_.append(blanks_);
      blanks_.clear();
    } else if (skip_blanks_) {
      blanks_.clear();
      skip_blanks_ = false;
    }
    current_value_.push_back(c);
    return true;
  }
  return false;
}

bool response_parser::parse_header_newline(char c, response &resp)
{
  if (c == '\n') {
    return true;
  } else if (c == ' ' || c == '\t') {
    state_ = HEADER_VALUE;
    skip_blanks_ = true;
    return true;
  } else if (isalnum(c) || ispunct(c)) {
    insert_header(current_key_, current_value_, resp);
    current_key_.clear();
    current_key_.push_back(c);
    state_ = HEADER_KEY;
    return true;
  } else if (c == '\r') {
    insert_header(current_key_, current_value_, resp);
    current_key_.clear();
    state_ = HEADER_FINISH;
    return true;
  }
  return false;
}

bool response_parser::parse_header_finish(char c)
{
  auto header_finished = c == '\n';
  if (header_finished) {
    state_ = BODY;
  }
  return header_finished;
}

bool response_parser::is_url_char(char c) const
{
  return isalnum(c) || strchr(URL_SPECIAL_CHAR, c) != nullptr;
}

bool response_parser::is_hex_char(char c) const
{
  return isalnum(c) || strchr(HEX_CHAR, c) != nullptr;
}

void response_parser::insert_header(const std::string &key, const std::string &value, response &resp)
{
  resp.headers_.insert(std::make_pair(key, value));
  if (strcasecmp(key.c_str(), response_header::CONTENT_TYPE) == 0) {
    resp.content_.type = value;
  } else if (strcasecmp(key.c_str(), response_header::CONTENT_LENGTH) == 0) {
    resp.content_.length = value;
  } else if (strcasecmp(key.c_str(), response_header::CONTENT_MD5) == 0) {
    resp.content_.md5 = value;
  }
}

void response_parser::reset()
{
  state_ = VERSION;
  http_prefix_index_ = 0;
  current_key_.clear();
  current_value_.clear();
  blanks_.clear();
  skip_blanks_ = false;
}

}
}