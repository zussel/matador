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

class json_serializer
{
public:

  json_serializer() = default;

  explicit json_serializer(json_format format);

  template < class T >
  std::string to_json(const T &obj)
  {
    json_.clear();
    write_value(obj);
    return json_;
  }

  template < class V >
  void serialize(const char *id, V &value)
  {
    write_id(id);
    write_value(value);
  }

  // string
  void serialize(const char *id, std::string &val, size_t);


  template < class V >
  void serialize(V &obj, typename std::enable_if<!matador::is_builtin<V>::value>::type* = 0)
  {
    matador::access::serialize(*this, obj);
  }

  template< class V >
  void write_value(identifier<V> &pk, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    append(pk).append(",");
    newline();
  }

  template < class V >
  void write_value(V &obj, typename std::enable_if<!matador::is_builtin<V>::value>::type* = 0)
  {
    append(obj);
    newline();
  }

  void write_value(identifier<std::string> &pk);

  template < int SIZE, class V >
  void write_value(identifier<varchar<SIZE, V>> &pk)
  {
    append(pk.value()).append(",");
    newline();
  }

  // numbers
  template < class V >
  void write_value(V &val, typename std::enable_if<std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    append(val).append(",");
    newline();
  }

  // boolean
  void write_value(bool &val);

  // string
  void write_value(std::string &val);

  // date
  void write_value(date &d);

  // time
  void write_value(time &t);

  template < class V >
  void write_value(std::list<V> &cont, typename std::enable_if<!std::is_class<V>::value>::type* = 0)
  {
    json_.append("[");
    join(cont, ", ").append("],");
    newline();
  }
  template < class V >
  void write_value(std::list<V> &cont, typename std::enable_if<std::is_class<V>::value>::type* = 0)
  {
    json_.append("[");
    join(cont, "");
    auto idx = json_.find_last_of(',');
    json_.erase(idx);
    json_.append("],");
    newline();
  }

  template < class V >
  void write_value(std::vector<V> &cont, typename std::enable_if<!std::is_class<V>::value>::type* = 0)
  {
    json_.append("[");
    join(cont, ", ").append("],");
    newline();
  }
  template < class V >
  void write_value(std::vector<V> &cont, typename std::enable_if<std::is_class<V>::value>::type* = 0)
  {
    json_.append("[");
    join(cont, "");
    auto idx = json_.find_last_of(',');
    json_.erase(idx);
    json_.append("],");
    newline();
  }

  template < class V >
  void write_value(std::set<V> &cont)
  {
    json_.append("[");
    join(cont, ", ").append("],\n");
  }

  template < class V >
  void write_value(std::unordered_set<V> &cont)
  {
    json_.append("[");
    join(cont, ", ").append("],");
    newline();
  }

private:
  std::string& write_id(const char *id);

  template < class R >
  std::string& join(R &range, const std::string &delim)
  {
    if (range.size() < 2) {
      for (const auto &obj : range) {
        append(obj);
      }
    } else {
      auto it = range.begin();
      append(*it++);
      for (;it != range.end(); ++it) {
        json_.append(delim);
        append(*it);
      }
    }
    return json_;
  }

  void begin_object();

  void end_object();

  void indent();

  void newline();

  std::string& append(const char *str);

  std::string& append(const std::string &str);

  std::string& append(std::string &val, size_t);

  std::string& append(const bool &value);

  template<class V>
  std::string& append(const V &value, typename std::enable_if<std::is_arithmetic<V>::value && !std::is_same<V, bool>::value>::type* = 0)
  {
    json_.append(std::to_string(value));
    return json_;
  }

  template < class V >
  std::string& append(V &obj, typename std::enable_if<!matador::is_builtin<V>::value>::type* = 0)
  {
    begin_object();
    matador::access::serialize(*this, obj);
    auto idx = json_.find_last_of(',');
    json_.erase(idx, 1);
    end_object();
    return json_;
  }

private:
  std::string json_;
  json_format format_;
  unsigned depth_ = 0;
};

}
#endif //MATADOR_JSON_SERIALIZER_HPP
