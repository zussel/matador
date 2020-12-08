#ifndef MATADOR_JSON_SERIALIZER_HPP
#define MATADOR_JSON_SERIALIZER_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/identifier.hpp"
#include "matador/utils/string.hpp"
#include "matador/utils/json.hpp"
#include "matador/utils/is_builtin.hpp"

#include <set>
#include <unordered_set>
#include <sstream>

namespace matador {

struct json_format
{
public:
  json_format() = default;
  explicit json_format(bool enable_line_break)
    : enable_line_break_(enable_line_break)
  {}
  explicit json_format(unsigned indentation)
    : indentation_(indentation)
  {}
  json_format(bool enable_line_break, unsigned indentation)
    : enable_line_break_(enable_line_break)
    , indentation_(indentation)
  {}

  bool show_line_break() const
  {
    return enable_line_break_;
  }

  unsigned indentation() const
  {
    return indentation_;
  }

  static json_format compact;
  static json_format pretty;

private:
  bool enable_line_break_ = true;
  unsigned indentation_ = 2;
};

json_format json_format::compact { false, 0 }; /* NOLINT */
json_format json_format::pretty { true, 2 }; /* NOLINT */

class json_serializer
{
public:

  json_serializer() = default;

  explicit json_serializer(json_format format);

  template < class T >
  std::string to_string(const T &obj)
  {
    serialize(obj);
    return json_;
  }

  template < class V >
  void serialize(const char *id, V &obj, typename std::enable_if<std::is_class<V>::value>::type* = 0)
  {
    write_id(id);
    serialize(obj);
  }

  template< class V >
  void serialize(const char *id, identifier<V> &pk, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    write_id(id).append(pk);
    newline();
  }

  void serialize(const char *id, identifier<std::string> &pk);

  template < int SIZE, class V >
  void serialize(const char *id, identifier<varchar<SIZE, V>> &pk)
  {
    write_id(id).append("\"").append(pk).append("\",");
    newline();
  }

  // numbers
  template < class V >
  void serialize(const char *id, V &val, typename std::enable_if<std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    write_id(id).append(std::to_string(val)).append(",");
    newline();
  }

  // boolean
  void serialize(const char *id, bool &val);

  // string
  void serialize(const char *id, std::string &val, size_t);

  // string
  void serialize(const char *id, std::string &val);

  // date
  void serialize(const char *id, date &d);

  // time
  void serialize(const char *id, time &t);

  template < class V >
  void serialize(const char *id, std::list<V> &cont, typename std::enable_if<!std::is_class<V>::value>::type* = 0)
  {
    write_id(id).append("[");
    join_builtin(cont, ", ").append("],");
    newline();
  }
  template < class V >
  void serialize(const char *id, std::list<V> &cont, typename std::enable_if<std::is_class<V>::value>::type* = 0)
  {
    write_id(id).append("[");
    join_object(cont, "");
    auto idx = json_.find_last_of(',');
    json_.erase(idx);
    json_.append("],");
    newline();
  }

  template < class V >
  void serialize(const char *id, std::vector<V> &cont, typename std::enable_if<!std::is_class<V>::value>::type* = 0)
  {
    write_id(id).append("[");
    join_builtin(cont, ", ").append("],");
    newline();
  }
  template < class V >
  void serialize(const char *id, std::vector<V> &cont, typename std::enable_if<std::is_class<V>::value>::type* = 0)
  {
    write_id(id).append("[");
    join_object(cont, "");
    auto idx = json_.find_last_of(',');
    json_.erase(idx);
    json_.append("],");
    newline();
  }

  template < class V >
  void serialize(const char *id, std::set<V> &cont)
  {
//    write_id(id).append("[");
//    join_builtin(cont, ", ").append("],\n");
  }

  template < class V >
  void serialize(const char *id, std::unordered_set<V> &cont)
  {
    write_id(id).append("[");
    join_builtin(cont, ", ").append("],");
    newline();
  }

private:
  template < class V >
  void serialize(V &obj, typename std::enable_if<!matador::is_builtin<V>::value>::type* = 0)
  {
    begin_object();
    matador::access::serialize(*this, obj);
    auto idx = json_.find_last_of(',');
    json_.erase(idx, 1);
    end_object();
  }

  std::string& write_id(const char *id);

  template < class R >
  std::string& join_object(R &range, const std::string &delim)
  {
    if (range.size() < 2) {
      for (const auto &obj : range) {
        serialize(obj);
      }
    } else {
      auto it = range.begin();
      serialize(*it++);
      auto idx = json_.find_last_of('\n');
      json_.erase(idx, 1);
      for (;it != range.end(); ++it) {
        json_.append(delim);
        serialize(*it);
        idx = json_.find_last_of('\n');
        json_.erase(idx, 1);
      }
    }
    return json_;
  }

  template < class R >
  std::string& join_builtin(R &range, const std::string &delim)
  {
    if (range.size() < 2) {
      for (const auto &obj : range) {
        json_.append(std::to_string(obj));
      }
    } else {
      auto it = range.begin();
      json_.append(std::to_string(*it++));
      for (;it != range.end(); ++it) {
        json_.append(delim).append(std::to_string(*it));
      }
    }
    return json_;
  }

  void begin_object();

  void end_object();

  void indent();

  void newline();

private:
  std::string json_;
  json_format format_;
  unsigned depth_ = 0;
};

}
#endif //MATADOR_JSON_SERIALIZER_HPP
