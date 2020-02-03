#ifndef REACTOR_JSON_HPP
#define REACTOR_JSON_HPP

#ifdef _MSC_VER
#ifdef matador_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_UTILS_API
#endif

#include <map>
//#include <unordered_map>
#include <memory>
#include <utility>
#include <vector>
#include <algorithm>
#include <ostream>
#include <vector>

namespace matador {

template < class R >
void join(R &range, std::ostream &out, const std::string &delim)
{
  if (range.size() < 2) {
    for (auto &i : range) {
      out << i;
    }
  } else {
    auto it = range.begin();
    out << *it++;
    for (;it != range.end(); ++it) {
      out << delim << *it;
    }
  }
}

template < class K, class V >
void join(std::map<K, V> &range, std::ostream &out, const std::string &delim)
//void join(std::unordered_map<K, V> &range, std::ostream &out, const std::string &delim)
{
  if (range.size() < 2) {
    for (auto &i : range) {
      out << "\"" << i.first << "\": " << i.second;
    }
  } else {
    auto it = range.begin();
    out << "\"" << it->first << "\": " << it->second;
    for (++it ;it != range.end(); ++it) {
      out << delim << "\"" << it->first << "\": " << it->second;
    }
  }
}

template < class JSON_TYPE >
class json_iterator;

struct OOS_UTILS_API json
{
  typedef json_iterator<json> iterator;

  enum json_type
  {
    e_null,
    e_integer,
    e_real,
    e_boolean,
    e_string,
    e_object,
    e_array,
  };

  json();

  template<class T>
  json(T val, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type * = 0)
    : value_((long long)val), type(e_integer)
  {}

  template<class T>
  json(T val, typename std::enable_if<std::is_floating_point<T>::value>::type * = 0)
   : value_(val), type(e_real)
  {}

  template<class T>
  json(T val, typename std::enable_if<std::is_convertible<T, std::string>::value>::type * = 0)
   : value_(val), type(e_string)
  {}

  json(bool b);

  json(std::initializer_list<json> l);

  json(std::nullptr_t n);

  explicit json(json_type t);

  ~json();

  static json object();

  static json array();

  template < class T >
  json& operator=(T val) {
    reset(val);
    return *this;
  }

  json(const json &x);

  json(json &&x) noexcept;

  json& operator=(json &&x) noexcept;

  json& operator=(const json &x);

  friend std::ostream& operator<<(std::ostream &out, json &val);

  void dump(std::ostream &out) const;

  void copy_from(const json &x);

  json& operator[](const std::string &key) {
    if (type != e_object) {
      clear();
      value_.object = new object_type;
      type = e_object;
    }
    auto it = value_.object->insert(std::make_pair(key, json())).first;
    return it->second;
  }

  json& operator[](const char *key);

  json& operator[](std::size_t i);
  const json& operator[](std::size_t i) const {
    if (type != e_array) {
      return *this;
    }
    if (i >= value_.array->size()) {
      throw std::logic_error("index out of bounce");
    }
    return value_.array->at(i);
  }

  template < class T >
  void push_back(const T &val) {
    if (type != e_array) {
      clear();
      value_.array = new std::vector<json>;
      type = e_array;
    }
    value_.array->push_back(val);
  }

  json& back();
  const json& back() const;

  template < class T >
  void reset(T val, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type * = 0)
  {
    if (type != e_integer) {
      clear();
      type = e_integer;
    }
    value_.integer = val;
  }

  template < class T >
  void reset(T val, typename std::enable_if<std::is_floating_point<T>::value>::type * = 0)
  {
    if (type != e_real) {
      clear();
      type = e_real;
    }
    value_.real = val;
  }

  template < class T >
  void reset(T val, typename std::enable_if<std::is_convertible<T, std::string>::value>::type * = 0)
  {
    if (type != e_string) {
      clear();
      type = e_string;
    }
    if (value_.str == nullptr) {
      value_.str = new std::string(val);
    } else {
      value_.str->assign(val);
    }
  }

  void reset(bool b);

  std::size_t size() const;

  bool empty() const;

  iterator begin();
  iterator end();

  template < class T >
  typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value, T>::type
  as() const {
    throw_on_wrong_type(e_integer);
    return static_cast<T>(value_.integer);
  }

  template < class T >
  typename std::enable_if<std::is_floating_point<T>::value, T>::type
  as() const {
    throw_on_wrong_type(e_real);
    return static_cast<T>(value_.real);
  }

  template < class T >
  typename std::enable_if<std::is_same<bool, T>::value, T>::type
  as() const {
    throw_on_wrong_type(e_boolean);
    return static_cast<T>(value_.boolean);
  }

  template < class T >
  typename std::enable_if<std::is_convertible<T, std::string>::value, T>::type
  as() const {
    throw_on_wrong_type(e_string);
    return *value_.str;
  }

  bool is_number() const;

  bool is_real() const;

  bool is_integer() const;

  bool is_boolean() const;

  bool is_string() const;

  bool is_array() const;

  bool is_object() const;

  bool is_null() const;

  template < class T >
  T at(std::size_t pos) const {
    if (type != e_array) {
      throw std::logic_error("type doesn't provide at()");
    }
    return value_.array->at(pos).as<T>();
  }

  void clear()
  {
    switch (type) {
      case e_string:
        delete value_.str;
        break;
      case e_object:
        delete value_.object;
        break;
      case e_array:
        delete value_.array;
        type = e_null;
        break;
      default:
        type = e_null;
        break;
    }

    value_ = {};
  }

  void erase(const std::string &key);
  void erase(std::size_t i);

  void throw_on_wrong_type(json_type t) const;

  //typedef std::unordered_map<std::string, json> object_type;
  typedef std::map<std::string, json> object_type;
  typedef std::vector<json> array_type;

  union json_value {
    json_value() : integer(0) {}
    explicit json_value(long long l) : integer(l) {}
    explicit json_value(double d) : real(d) {}
    explicit json_value(bool b) : boolean(b) {}
    explicit json_value(const char *val) : str(new std::string(val)) {}
    explicit json_value(std::string s) : str(new std::string(std::move(s))) {}

    long long integer;
    double real;
    bool boolean;
    std::string *str;
    object_type *object;
    array_type *array;
  };

  json_value value_;
  json_type type = e_integer;
};

template < class JSON_TYPE >
class json_iterator
{
public:
  typedef JSON_TYPE json_type;
  typedef typename json_type::array_type array_type;
  typedef typename json_type::object_type object_type;

  json_iterator(json_type *ptr, bool as_begin)
    : obj_(ptr)
  {
    switch(obj_->type) {
      case json::e_object:
        iter.object_iterator = typename object_type::iterator();
        iter.object_iterator = (as_begin ? obj_->value_.object->begin() : obj_->value_.object->end());
        break;
      case json::e_array:
        iter.array_iterator = typename array_type::iterator();
        iter.array_iterator = (as_begin ? obj_->value_.array->begin() : obj_->value_.array->end());
        break;
      default:
        it = as_begin ? begin_ : end_;
        break;
    }
  }

  json_iterator(const json_iterator<JSON_TYPE> &x)
    : obj_(x.obj_), iter(x.iter)
  {}

  json_iterator& operator=(const json_iterator<JSON_TYPE> &x)
  {
    if (this != &x) {
      obj_ = x.obj_;
      iter = x.iter;
    }
    return *this;
  }

  /**
   * (++i)
   * @return
   */
  json_iterator operator++() {
    switch(obj_->type) {
      case json::e_object:
        ++iter.object_iterator;
        break;
      case json::e_array:
        ++iter.array_iterator;
        break;
      default:
        ++it;
        break;
    }
    return *this;
  }

  /**
   * (i++)
   * @return
   */
  json_iterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

    /**
     * (--i)
     * @return
     */
  json_iterator operator--() {
    switch(obj_->type) {
      case json::e_object:
        --iter.object_iterator;
        break;
      case json::e_array:
        --iter.array_iterator;
        break;
      default:
        --it;
        break;
    }
    return *this;
  }

  /**
 * (i--)
 * @return
 */
  json_iterator operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  json& operator*() const {
    switch(obj_->type) {
      case json::e_object:
        return iter.object_iterator->second;
      case json::e_array:
        return *iter.array_iterator;
      case json::e_null:
        throw std::logic_error("json null hasn't a value");
      default:
        return *obj_;
    }
  }

  json* operator->() const {
    switch(obj_->type) {
      case json::e_object:
        return &(iter.object_iterator->second);
      case json::e_array:
        return &*iter.array_iterator;
      case json::e_null:
        throw std::logic_error("json null hasn't a value");
      default:
        return obj_;
    }
  }

  bool operator==(const json_iterator<JSON_TYPE> &x) const
  {
    switch(obj_->type) {
      case json::e_object:
        return iter.object_iterator == x.iter.object_iterator;
      case json::e_array:
        return iter.array_iterator == x.iter.array_iterator;
      default:
        return it == x.it;
    }
  }

  bool operator!=(const json_iterator<JSON_TYPE> &x) const
  {
    return !operator==(x);
  }

private:
  static const std::size_t begin_ = 0;
  static const std::size_t end_ = 1;

  json *obj_;

  struct iter_type {
    typename object_type::iterator object_iterator;
    typename array_type::iterator array_iterator;
  };

  iter_type iter;
  // for builtin types (number, bool, string)
  std::size_t it = 0;
};

OOS_UTILS_API std::string to_string(const matador::json &j);

}

#endif //REACTOR_JSON_HPP
