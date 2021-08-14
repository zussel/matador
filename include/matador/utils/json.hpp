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
#include <memory>
#include <utility>
#include <vector>
#include <algorithm>
#include <ostream>
#include <vector>

namespace matador {

class OOS_UTILS_API json_format
{
public:
  json_format() = default;
  explicit json_format(bool enable_line_break);
  explicit json_format(unsigned indentation);
  json_format(bool enable_line_break, bool skip_empty, unsigned indentation);

  bool show_line_break() const;

  bool skip_empty();

  unsigned indentation() const;
  char indentation_char() const;

  static json_format compact;
  static json_format pretty;

private:
  bool enable_line_break_ = true;
  bool skip_empty_ = true;
  unsigned indentation_ = 2;
  char indentation_char_ = ' ';
};

/**
 * Joins a range of elements as string within a list
 * with a given delimiter and writes it to the
 * given stream
 *
 * @tparam R Type og the range (e.g. map, list, vector, etc)
 * @param range The range with the elements to join
 * @param out The stream to write on
 * @param delim The delimiter for the elements
 * @return The ostream reference
 */
template < class R >
std::ostream& join(R &range, std::ostream &out, const std::string &delim)
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
  return out;
}

/**
 * Joins a range of elements as string within a list
 * with a given delimiter and writes it to the
 * given stream
 *
 * @tparam R Type og the range (e.g. map, list, vector, etc)
 * @param range The range with the elements to join
 * @param out The stream to write on
 * @param delim The delimiter for the elements
 * @return The ostream reference
 */
template < class R >
std::string join(R &range, const std::string &delim)
{
  std::string result {};
  if (range.size() < 2) {
    for (const auto &i : range) {
      result += to_string(i);
    }
  } else {
    auto it = range.begin();
    result += to_string(*it++);
    for (;it != range.end(); ++it) {
      result += delim + to_string(*it);
    }
  }
  return result;
}

/**
 * Joins a map of key value pairs as string within
 * a list with a given delimiter and writes it to
 * the given stream.
 *
 * The key value pairs will be written in a
 * json style: ("key": value)
 *
 * @tparam K Type of key
 * @tparam V Type of value
 * @param range The range with the elemets to join
 * @param out The stream to write on
 * @param delim The delimiter for the elements
 * @return The ostream reference
 */
template < class K, class V >
std::ostream& join(std::map<K, V> &range, std::ostream &out, const std::string &delim)
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
  return out;
}

/**
 * Joins a map of key value pairs as string within
 * a list with a given delimiter and writes it to
 * the given stream.
 *
 * The key value pairs will be written in a
 * json style: ("key": value)
 *
 * @tparam K Type of key
 * @tparam V Type of value
 * @param range The range with the elemets to join
 * @param out The stream to write on
 * @param delim The delimiter for the elements
 * @return The ostream reference
 */
template < class K, class V >
std::string join(std::map<K, V> &range, const std::string &delim)
{
  std::string result {};
  if (range.size() < 2) {
    for (auto &i : range) {
      result += "\"" + i.first + "\": " + to_string(i.second);
    }
  } else {
    auto it = range.begin();
    result += "\"" + it->first + "\": " + to_string(it->second);
    for (++it ;it != range.end(); ++it) {
      result += delim + "\"" + it->first + "\": " + to_string(it->second);
    }
  }
  return result;
}

template < class JSON_TYPE >
class json_iterator;

template < class JSON_TYPE >
class const_json_iterator;

/**
 * @brief This class represents either a json value an object or an array
 *
 * This class represents any json value
 * - number (double or long long)
 * - string
 * - boolean
 * - object
 * - array
 *
 *
 */
class OOS_UTILS_API json
{
public:
  typedef json_iterator<json> iterator;             /**< Shortcut to json iterator type */
  typedef const_json_iterator<json> const_iterator; /**< Shortcut to json const iterator type */
  typedef std::map<std::string, json> object_type;  /**< Shortcut to json object map type */
  typedef std::vector<json> array_type;             /**< Shortcut to json array vector type */

  /**
   * Enum representing the different json value types
   */
  enum json_type
  {
    e_null,     /**< Json null type */
    e_integer,  /**< Json integer type (number) */
    e_real,     /**< Json real type (number) */
    e_boolean,  /**< Json boolean type */
    e_string,   /**< Json string type */
    e_object,   /**< Json object type */
    e_array,    /**< Json array type */
  };

  /**
   * Creates an json null value
   */
  json();

  /**
   * Creates a number json value where the
   * value is treated like an integer of type long long
   *
   * @tparam T Type of the value
   * @param val Value of the json object
   */
  template<class T>
  json(T val, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type * = 0)
    : value_((long long)val), type_(e_integer)
  {}

  /**
   * Creates a number json value where the
   * value is treated like a floating point of type double
   *
   * @tparam T Type of the value
   * @param val Value of the json object
   */
  template<class T>
  json(T val, typename std::enable_if<std::is_floating_point<T>::value>::type * = 0)
   : value_(val), type_(e_real)
  {}

  /**
   * Creates a string json value where the
   * value is treated like std::string
   *
   * @tparam T Type of the value
   * @param val Value of the json object
   */
  template<class T>
  json(T val, typename std::enable_if<std::is_convertible<T, std::string>::value>::type * = 0)
   : value_(val), type_(e_string)
  {}

  /**
   * Creates a boolean json value
   *
   * @param b The boolean value of the json object
   */
  json(bool b);

  /**
   * Creates a json array from the initializer list.
   *
   * Note: If the list contains just two elements and the first element
   * is of type string an json object is created.
   *
   * @param l
   */
  json(std::initializer_list<json> l);

  /**
   * Creates a null value json object
   *
   * @param n Null value
   */
  json(std::nullptr_t n);

  /**
   * Creates explicitly a json object of
   * the give json type
   *
   * @param t Type of the json object
   * @sa json_type
   */
  explicit json(json_type t);

  /**
   * Destroys the json object
   */
  ~json();

  /**
   * Creates an empty json object.
   *
   * @return An empty json object
   */
  static json object();

  /**
   * Creates an empty json array.
   *
   * @return An empty json array
   */
  static json array();

  /**
   * Assigns a value to the json object
   * On json type change the changes
   * are adjusted
   *
   * @tparam T Type of the value
   * @param val Value to assign
   * @return The assigned json object
   */
  template < class T >
  json& operator=(T val) {
    reset(val);
    return *this;
  }

  /**
   * Copy construct a josn object from
   * the given json object
   *
   * @param x The json object to copy from
   */
  json(const json &x);

  /**
   * Copy move the given json object
   *
   * @param x The json object to copy move
   */
  json(json &&x) noexcept;

  /**
   * Move assign the given json object
   *
   * @param x The json object to move assign
   * @return The move assigned json object
   */
  json& operator=(json &&x) noexcept;

  /**
   * Copy assign the given json object
   *
   * @param x The json object to copy assign
   * @return The copied json object
   */
  json& operator=(const json &x);

  /**
   * Compares equality of two json objects
   *
   * @param a First json object to compare
   * @param b Second json object to compare
   * @return True if both json objects are equal
   */
  friend OOS_UTILS_API bool operator==(const json &a, const json &b);

  /**
   * Compares equality of two json objects
   *
   * @param a First json object to compare
   * @param b Second json object to compare
   * @return True if both json objects are not equal
   */
  friend OOS_UTILS_API bool operator!=(const json &a, const json &b);

  /**
   * Compares two json objects if first object
   * is less than second.
   *
   * @param a First json object to compare
   * @param b Second json object to compare
   * @return True if first json object is less than second
   */
  friend OOS_UTILS_API bool operator<(const json &a, const json &b);

  /**
   * Compares two json objects if first object
   * is less than or equal second.
   *
   * @param a First json object to compare
   * @param b Second json object to compare
   * @return True if first json object is less than or equal second
   */
  friend OOS_UTILS_API bool operator<=(const json &a, const json &b);

  /**
   * Compares two json objects if first object
   * is greater than second.
   *
   * @param a First json object to compare
   * @param b Second json object to compare
   * @return True if first json object is greater than second
   */
  friend OOS_UTILS_API bool operator>(const json &a, const json &b);

  /**
   * Compares two json objects if first object
   * is greater than or equal second.
   *
   * @param a First json object to compare
   * @param b Second json object to compare
   * @return True if first json object is greater than or equal second
   */
  friend OOS_UTILS_API bool operator>=(const json &a, const json &b);

  template < class T, typename std::enable_if<std::is_scalar<T>::value>::type>
  friend bool operator<(const json &a, const T &b);
  template < class T, typename std::enable_if<std::is_scalar<T>::value>::type>
  friend bool operator<(const T &a, const json &b);

  template < class T, typename std::enable_if<std::is_scalar<T>::value>::type>
  friend bool operator<=(const json &a, const T &b);
  template < class T, typename std::enable_if<std::is_scalar<T>::value>::type>
  friend bool operator<=(const T &a, const json &b);

  template < class T, typename std::enable_if<std::is_scalar<T>::value>::type>
  friend bool operator>(const json &a, const T &b);
  template < class T, typename std::enable_if<std::is_scalar<T>::value>::type>
  friend bool operator>(const T &a, const json &b);

  template < class T, typename std::enable_if<std::is_scalar<T>::value>::type>
  friend bool operator>=(const json &a, const T &b);
  template < class T, typename std::enable_if<std::is_scalar<T>::value>::type>
  friend bool operator>=(const T &a, const json &b);

  /**
   * Creates a string of the json object
   *
   * @return Json as string
   */
  std::string str() const;

  /**
   * Creates a string of the json object
   * with the given json format
   *
   * @param format Format object defining the json output
   * @return Json as string
   */
  std::string str(const json_format &format) const;

  /**
   * Print the json object to the given ostream
   *
   * @param out The stream to write on
   * @param val The json object to write
   * @return The written on stream
   */
  friend OOS_UTILS_API std::ostream& operator<<(std::ostream &out, const json &val);

  /**
   * Print the json object to the given ostream
   *
   * @param out The stream to write on
   */
  void dump(std::ostream &out) const;

  /**
   * Get the json object of the given key.
   * If the json type isn't object the type
   * is changed to object and a json null
   * value is inserted at key
   *
   * @param key The key of the requested value
   * @return The requested value for the given key
   */
  json& operator[](const std::string &key);

  /**
   * Get the json object of the given key.
   * If the json type isn't object the type
   * the object itself is returned (this)
   *
   * @param key The key of the requested value
   * @return The requested value for the given key
   */
  const json& operator[](const std::string &key) const;

  /**
   * Returns true if json is of type object
   * and contains the given key
   *
   * @param key Key to check
   * @return True if key is available
   */
  bool contains(const std::string &key) const;

  /**
   * Get the json object at given index i
   * if the json if of type array.
   * 
   * If the json isn't of type array a reference
   * the this json is returned.
   * If the index is out of bound a logic_error is
   * thrown
   * 
   * @param i Index of the requested value
   * @return The requested value for the given index
   * @throws std::logic_error If the index is out of bounce
   */
  json& operator[](std::size_t i);

  /**
   * Get the json object at given index i
   * if the json if of type array.
   *
   * If the json isn't of type array a reference
   * the this json is returned.
   * If the index is out of bound a logic_error is
   * thrown
   *
   * @param i Index of the requested value
   * @return The requested value for the given index
   * @throws std::logic_error If the index is out of bounce
   */
  const json& operator[](std::size_t i) const;

  /**
   * Push back the given value to the json array
   *
   * If the current type of the json object isn't
   * array the type is changed to array.
   *
   * @tparam T The type of the value to push back
   * @param val The value to push back
   */
  template < class T >
  void push_back(const T &val) {
    if (type_ != e_array) {
      clear();
      value_.array = new std::vector<json>;
      type_ = e_array;
    }
    value_.array->push_back(val);
  }

  /**
   * Returns a reference to the last json object
   * of the array.
   *
   * If the current type isn't array an exception
   * is thrown.
   * @throws std::logic_error Throws a std::logic_error
   * if the current type isn't of type array
   * @return The reference of the last json object in the array
   */
  json& back();

  /**
   * Returns a const reference to the last json object
   * of the array.
   *
   * If the current type isn't array an exception
   * is thrown.
   * @throws std::logic_error Throws a std::logic_error
   * if the current type isn't of type array
   * @return The const reference of the last json object in the array
   */
  const json& back() const;

  /**
   * Resets the json value to a new integer (number) type value.
   *
   * @tparam T Type of the new value
   * @param val Value to set
   */
  template < class T >
  void reset(T val, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type * = 0)
  {
    if (type_ != e_integer) {
      clear();
      type_ = e_integer;
    }
    value_.integer = val;
  }

  /**
   * Resets the json value to a new floating point (number) type value.
   *
   * @tparam T Type of the new value
   * @param val Value to set
   */
  template < class T >
  void reset(T val, typename std::enable_if<std::is_floating_point<T>::value>::type * = 0)
  {
    if (type_ != e_real) {
      clear();
      type_ = e_real;
    }
    value_.real = val;
  }

  /**
   * Resets the json value to a new string type value.
   *
   * @tparam T Type of the new value
   * @param val Value to set
   */
  template < class T >
  void reset(T val, typename std::enable_if<std::is_convertible<T, std::string>::value>::type * = 0)
  {
    if (type_ != e_string) {
      clear();
      type_ = e_string;
    }
    if (value_.str == nullptr) {
      value_.str = new std::string(val);
    } else {
      value_.str->assign(val);
    }
  }

  /**
   * Resets the json value to a new boolean type value.
   *
   * @param b Value to set
   */
  void reset(bool b);

  /**
   * Returns either the size of the json array or
   * the size of a json object. If the type is null then
   * 0 (zero) is returned. For all other types 1 (one)
   * is returned
   *
   * @return The size (count of elements)
   */
  std::size_t size() const;

  /**
   * Returns the corresponding result of empty()
   * if json is of type array or of type object.
   * If json is of type null then false is returned.
   * For all other types true is returned.
   *
   * @return True if array or object is empty
   */
  bool empty() const;

  /**
   * Returns the begin iterator of the json array
   * or the json object.
   * If the json isn't of type array or object this
   * json object is return in the iterator
   * @sa json_iterator
   * 
   * @return The begin iterator
   */
  iterator begin();

  /**
   * Returns the begin iterator of the json array
   * or the json object.
   * If the json isn't of type array or object this
   * json object is return in the iterator
   * @sa json_iterator
   *
   * @return The begin iterator
   */
  const_iterator begin() const;

  /**
   * Returns the end iterator of the json array
   * or the json object.
   * If the json isn't of type array or object this
   * json object is return in the iterator
   * @sa json_iterator
   * 
   * @return The end iterator
   */
  iterator end();

  /**
   * Returns the end iterator of the json array
   * or the json object.
   * If the json isn't of type array or object this
   * json object is return in the iterator
   * @sa json_iterator
   *
   * @return The end iterator
   */
  const_iterator end() const;

  /**
   * Return the json value as an integral type
   * 
   * If json type isn't integer a std::logic_error is thrown
   * 
   * @throws std::logic_error If the type isn't integral
   * @return The json value as requested integral type
   */
  template < class T >
  typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value, T>::type
  as() const {
    throw_on_wrong_type(e_integer);
    return static_cast<T>(value_.integer);
  }

  /**
   * Return the json value as a floating point type
   * 
   * If json type isn't a floating point a std::logic_error is thrown
   * 
   * @throws std::logic_error If the type isn't floating point
   * @return The json value as requested floating point type
   */
  template < class T >
  typename std::enable_if<std::is_floating_point<T>::value, T>::type
  as() const {
    throw_on_wrong_type(e_real);
    return static_cast<T>(value_.real);
  }

  /**
   * Return the json value as a boolean
   * 
   * If json type isn't boolean a std::logic_error is thrown
   * 
   * @throws std::logic_error If the type isn't boolean
   * @return The json value as boolean
   */
  template < class T >
  typename std::enable_if<std::is_same<bool, T>::value, T>::type
  as() const {
    throw_on_wrong_type(e_boolean);
    return static_cast<T>(value_.boolean);
  }

  /**
   * Return the json value as a string
   * 
   * If json type isn't string a std::logic_error is thrown
   * 
   * @throws std::logic_error If the type isn't of type string
   * @return The json value as string
   */
  template < class T >
  typename std::enable_if<std::is_convertible<T, std::string>::value, T>::type
  as() const {
    throw_on_wrong_type(e_string);
    return *value_.str;
  }

  /**
   * Returns true if the given type is an integral type
   * 
   * @return True if given type is an integral type
   */
  template < class T >
  bool fits_to_type(typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type * = 0) const {
    return is_integer();
  }

  /**
   * Returns true if the given type is a floating point type
   * 
   * @return True if given type is a floating point type
   */
  template < class T >
  bool fits_to_type(typename std::enable_if<std::is_floating_point<T>::value>::type * = 0) const {
    return is_real();
  }

  /**
   * Returns true if the given type is a boolean
   * 
   * @return True if given type is a boolean
   */
  template < class T >
  bool fits_to_type(typename std::enable_if<std::is_same<bool, T>::value>::type * = 0) const {
    return is_boolean();
  }

  /**
   * Returns true if the given type is a string
   * 
   * @return True if given type is a string
   */
  template < class T >
  bool fits_to_type(typename std::enable_if<std::is_convertible<T, std::string>::value>::type * = 0) const {
    return is_string();
  }

  /**
   * Returns the json type enum
   *
   * @returns Json type enum
   */
  json_type type() const;

  /**
   * Returns true if json type is a number
   * 
   * @returns True if json type is a number
   */
  bool is_number() const;

  /**
   * Returns true if json type is a floating point type
   * 
   * @returns True if json type is a floating point type
   */
  bool is_real() const;

  /**
   * Returns true if json type is an integral type
   * 
   * @returns True if json type is a integral type
   */
  bool is_integer() const;

  /**
   * Returns true if json type is a boolean
   * 
   * @returns True if json type is a boolean
   */
  bool is_boolean() const;

  /**
   * Returns true if json type is a string
   * 
   * @returns True if json type is a string
   */
  bool is_string() const;

  /**
   * Returns true if json type is an array
   * 
   * @returns True if json type is an array
   */
  bool is_array() const;

  /**
   * Returns true if json type is an object
   * 
   * @returns True if json type is an object
   */
  bool is_object() const;

  /**
   * Returns true if json type is null
   * 
   * @returns True if json type is null
   */
  bool is_null() const;

  /**
   * Returns the json element at given position
   * if the current json is of type array.
   * 
   * if current json isn't of type array an
   * std::logic_error is thrown.
   * 
   * @param pos The position of the requested element
   * 
   * @throws std::logic_error If json isn't of type array
   * @return json object at given position
   */
  template < class T >
  T at(std::size_t pos) const {
    if (type_ != e_array) {
      throw std::logic_error("type doesn't provide at()");
    }
    return value_.array->at(pos).as<T>();
  }

  json& get(const std::string &key);
  const json& get(const std::string &key) const;

  json& at_path(const std::string &path, char delimiter = '.');
  const json& at_path(const std::string &path, char delimiter = '.') const;

  /**
   * Clears the current json object.
   * 
   * All json values are deleted and the
   * type is set to null
   */
  void clear()
  {
    switch (type_) {
      case e_string:
        delete value_.str;
        break;
      case e_object:
        delete value_.object;
        break;
      case e_array:
        delete value_.array;
        type_ = e_null;
        break;
      default:
        type_ = e_null;
        break;
    }

    value_ = {};
  }

  /**
   * Erases the json element with given key.
   * If json isn't of type object nothing is done.
   * 
   * @param key The key of the element to erase
   */
  void erase(const std::string &key);

  /**
   * Erases the json element at the given position.
   * If json isn't of type array nothing is done.
   * If the given position is out of bounce an
   * appropriate std::logic_error is thrown
   * 
   * @param i The position of the element to erase
   */
  void erase(std::size_t i);

private:
  void throw_on_wrong_type(json_type t) const;

  void copy_from(const json &x);

  friend class json_iterator<json>;
  friend class const_json_iterator<json>;

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
  json_type type_ = e_integer;
};

template<class T>
bool operator<(const json &a, const T &b)
{
  return a < json(b);
}

template<class T>
bool operator<(const T &a, const json &b)
{
  return json(a) < b;
}

template<class T>
bool operator<=(const json &a, const T &b)
{
  return a <= json(b);
}

template<class T>
bool operator<=(const T &a, const json &b)
{
  return json(a) <= b;
}

template<class T>
bool operator>(const json &a, const T &b)
{
  return a > json(b);
}

template<class T>
bool operator>(const T &a, const json &b)
{
  return json(a) > b;
}

template<class T>
bool operator>=(const json &a, const T &b)
{
  return a >= json(b);
}

template<class T>
bool operator>=(const T &a, const json &b)
{
  return json(a) >= b;
}

/**
 * @brief An iterator for json array and objects
 * 
 * The iterator handles elements of json arrays
 * and json objects.
 * If initialized with a non array or object the
 * iterator uses the given json as the element
 * neither as begin or end.
 */
template < class JSON_TYPE >
class json_iterator
{
public:
  typedef JSON_TYPE json_type; /**< Shortcut for json */
  typedef typename json_type::array_type array_type; /**< Shortcut for json array type */
  typedef typename json_type::object_type object_type; /**< Shortcut for json object type */

  /**
   * Creates a json_iterator for the given json object.
   * In case the json object isn't of type object or array
   * the given flag as_begin tells the c'tor if the jsn value
   * is treated like begin or end element.
   *
   * @param ptr Pointer to the json
   * @param as_begin True if non array/object should be treated as begin iterator
   */
  json_iterator(json_type *ptr, bool as_begin)
    : obj_(ptr)
  {
    switch(obj_->type_) {
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

  /**
   * Copy construct a json_iterator from given
   * iterator
   *
   * @param x The iterator to copy from
   */
  json_iterator(const json_iterator<JSON_TYPE> &x)
    : obj_(x.obj_), iter(x.iter)
  {}

  /**
   * Copy assigns a json_iterator from given
   * iterator
   *
   * @param x The iterator to copy assign from
   */
  json_iterator& operator=(const json_iterator<JSON_TYPE> &x)
  {
    if (this != &x) {
      obj_ = x.obj_;
      iter = x.iter;
    }
    return *this;
  }

  /**
   * Post increments (++i) the json_iterator
   * step to next array element or next
   * object element
   *
   * @return The incremented iterator
   */
  json_iterator operator++() {
    switch(obj_->type_) {
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
   * Pre increments (i++) the json_iterator
   * step to next array element or next
   * object element
   *
   * @return The incremented iterator
   */
  json_iterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  /**
   * Post decrements (--i) the json_iterator
   * step to previous array element or next
   * object element
   *
   * @return The decremented iterator
   */
  json_iterator operator--() {
    switch(obj_->type_) {
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
   * Pre decrements (i--) the json_iterator
   * step to previous array element or next
   * object element
   *
   * @return The decremented iterator
   */
  json_iterator operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  /**
   * Indirection operator. Returns the reference to the
   * json object represented by the iterator. If the json type
   * is null a std::logic_error is thrown.
   *
   * @return The reference of the json object
   */
  json& operator*() const {
    switch(obj_->type_) {
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

  /**
   * Address of operator. Returns the pointer to the
   * json object represented by the iterator. If the json type
   * is null a std::logic_error is thrown.
   *
   * @return The pointer of the json object
   */
  json* operator->() const {
    switch(obj_->type_) {
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

  /**
   * Equal operator for json_iterator. Depending on
   * the json type the values are compared.
   *
   * @param x The json_iterator to compare
   * @return True if the values are the same
   */
  bool operator==(const json_iterator<JSON_TYPE> &x) const
  {
    switch(obj_->type_) {
      case json::e_object:
        return iter.object_iterator == x.iter.object_iterator;
      case json::e_array:
        return iter.array_iterator == x.iter.array_iterator;
      default:
        return it == x.it;
    }
  }

  /**
   * Not equal operator for json_iterator. Depending on
   * the json type the values are compared.
   *
   * @param x The json_iterator to compare
   * @return True if the values are the not same
   */
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

/**
 * @brief An iterator for json array and objects
 *
 * The iterator handles elements of json arrays
 * and json objects.
 * If initialized with a non array or object the
 * iterator uses the given json as the element
 * neither as begin or end.
 */
template < class JSON_TYPE >
class const_json_iterator
{
public:
  typedef JSON_TYPE json_type; /**< Shortcut for json */
  typedef typename json_type::array_type array_type; /**< Shortcut for json array type */
  typedef typename json_type::object_type object_type; /**< Shortcut for json object type */

  /**
   * Creates a json_iterator for the given json object.
   * In case the json object isn't of type_ object or array
   * the given flag as_begin tells the c'tor if the jsn value
   * is treated like begin or end element.
   *
   * @param ptr Pointer to the json
   * @param as_begin True if non array/object should be treated as begin iterator
   */
  const_json_iterator(const json_type *ptr, bool as_begin)
    : obj_(ptr)
  {
    switch(obj_->type_) {
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

  /**
   * Copy construct a json_iterator from given
   * iterator
   *
   * @param x The iterator to copy from
   */
  const_json_iterator(const const_json_iterator<JSON_TYPE> &x)
    : obj_(x.obj_), iter(x.iter)
  {}

  /**
   * Copy assigns a json_iterator from given
   * iterator
   *
   * @param x The iterator to copy assign from
   */
  const_json_iterator& operator=(const const_json_iterator<JSON_TYPE> &x)
  {
    if (this != &x) {
      obj_ = x.obj_;
      iter = x.iter;
    }
    return *this;
  }

  /**
   * Post increments (++i) the json_iterator
   * step to next array element or next
   * object element
   *
   * @return The incremented iterator
   */
  const_json_iterator operator++() {
    switch(obj_->type_) {
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
   * Pre increments (i++) the json_iterator
   * step to next array element or next
   * object element
   *
   * @return The incremented iterator
   */
  const_json_iterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  /**
   * Post decrements (--i) the json_iterator
   * step to previous array element or next
   * object element
   *
   * @return The decremented iterator
   */
  const_json_iterator operator--() {
    switch(obj_->type_) {
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
   * Pre decrements (i--) the json_iterator
   * step to previous array element or next
   * object element
   *
   * @return The decremented iterator
   */
  const_json_iterator operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  /**
   * Indirection operator. Returns the reference to the
   * json object represented by the iterator. If the json type
   * is null a std::logic_error is thrown.
   *
   * @return The reference of the json object
   */
  const json& operator*() const {
    switch(obj_->type_) {
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

  /**
   * Address of operator. Returns the pointer to the
   * json object represented by the iterator. If the json type
   * is null a std::logic_error is thrown.
   *
   * @return The pointer of the json object
   */
  const json* operator->() const {
    switch(obj_->type_) {
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

  /**
   * Equal operator for json_iterator. Depending on
   * the json type the values are compared.
   *
   * @param x The json_iterator to compare
   * @return True if the values are the same
   */
  bool operator==(const const_json_iterator<JSON_TYPE> &x) const
  {
    switch(obj_->type_) {
      case json::e_object:
        return iter.object_iterator == x.iter.object_iterator;
      case json::e_array:
        return iter.array_iterator == x.iter.array_iterator;
      default:
        return it == x.it;
    }
  }

  /**
   * Not equal operator for json_iterator. Depending on
   * the json type the values are compared.
   *
   * @param x The json_iterator to compare
   * @return True if the values are the not same
   */
  bool operator!=(const const_json_iterator<JSON_TYPE> &x) const
  {
    return !operator==(x);
  }

private:
  static const std::size_t begin_ = 0;
  static const std::size_t end_ = 1;

  const json *obj_;

  struct iter_type {
    typename object_type::iterator object_iterator;
    typename array_type::iterator array_iterator;
  };

  iter_type iter;
  // for builtin types (number, bool, string)
  std::size_t it = 0;
};

OOS_UTILS_API std::string to_string(const json &j);

}

#endif //REACTOR_JSON_HPP
