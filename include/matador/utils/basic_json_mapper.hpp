#ifndef MATADOR_BASIC_JSON_MAPPER_HPP
#define MATADOR_BASIC_JSON_MAPPER_HPP

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

#include "matador/utils/json.hpp"
#include "matador/utils/json_parser.hpp"
#include "matador/utils/identifier.hpp"
#include "matador/utils/memory.hpp"
#include "matador/utils/time.hpp"

namespace matador {

namespace details {
/// @cond MATADOR_DEV
struct OOS_UTILS_API mapper_runtime {
  explicit mapper_runtime(json_cursor &c) : cursor(c) {}
  json value;
  std::string key;
  std::string object_key;
  std::string array_key;
  json_cursor &cursor;
  const char *json_array_cursor = nullptr;
};
/// @endcond
}

/**
 * @brief A base class for all kind of json mapping
 *
 * The class acts as a boilerplate for all concrete
 * json mapping classes (@sa json_mapper and @sa json_object_mapper)
 *
 * It uses internally a generic json parser in combination
 * with a given serialization class type S to map a json
 * string to an object of type T.
 *
 * @tparam T Type of the object to which the json string is mapped
 * @tparam S The internally used serialization class type
 */
template < class T, class S >
class basic_json_mapper : public generic_json_parser<basic_json_mapper<T, S> >
{
public:
  typedef generic_json_parser<basic_json_mapper<T, S> > base; /**< Shortcut to this base class */

  /**
   * Creates the basic json mapper
   */
  basic_json_mapper()
    : mapper_runtime_(this->cursor())
    , serializer_(mapper_runtime_)
  {}

  /**
   * Maps a given json string to an new object
   * of type T.
   *
   * @param str Json string to parse and map
   * @return The mapped object
   */
  T object_from_string(const char *str)
  {
    return object_from_string(str, true);
  }

  /**
   * Maps a given json string to the given
   * object of type T.
   *
   * @param str Json string to parse and map
   * @param obj The object to map
   */
  void object_from_string(const char *str, T *obj)
  {
    object_from_string(str, obj, true);
  }

  /**
   * Maps a given json string to an array of
   * objects of type type.
   * if the json string doesn't contain an array
   * a @sa json_exception is thrown
   *
   * @param str The json array string to parse
   * @return List of mapped objects
   */
  std::vector<T> array_from_string(const char *str)
  {
    return std::move(array_from_string(str, true));
  }

  /// @cond OOS_DEV
  T object_from_string(const char *str, bool check_for_eos);
  void object_from_string(const char *str, T *obj, bool check_for_eos);

  std::vector<T> array_from_string(const char *str, bool check_for_eos);

  void on_parse_object(bool check_for_eos);
  void on_begin_object();
  void on_object_key(const std::string &key);
  void on_end_object();

  void on_begin_array();
  void on_end_array();

  void on_string(const std::string &value);
  void on_integer(long long value);
  void on_real(double value);
  void on_bool(bool value);
  void on_null();

  const details::mapper_runtime& runtime_data() const
  {
    return mapper_runtime_;
  }
  /// @endcond OOS_DEV
protected:

  /// @cond OOS_DEV
  T object_;
  std::vector<T> array_;

  details::mapper_runtime mapper_runtime_;

  bool is_array_ = false;
  bool is_object_ = false;

  S serializer_;
  /// @endcond
};

/// @cond MATADOR_DEV
template<class T, class C>
T basic_json_mapper<T, C>::object_from_string(const char *str, bool check_for_eos)
{
  mapper_runtime_.key.clear();
  object_ = serializer_.template create<T>();
  this->parse_json_object(str, check_for_eos);
  return std::move(object_);
}

template<class T, class C>
void basic_json_mapper<T, C>::object_from_string(const char *str, T *obj, bool check_for_eos)
{
  mapper_runtime_.key.clear();
  object_ = serializer_.template create<T>();
  this->parse_json_object(str, check_for_eos);
  *obj = std::move(object_);
}

template<class T, class C>
std::vector<T> basic_json_mapper<T, C>::array_from_string(const char *str, bool check_for_eos)
{
  mapper_runtime_.key.clear();
  array_.clear();
  this->parse_json_array(str, check_for_eos);
  return array_;
}

template<class T, class C>
void basic_json_mapper<T, C>::on_parse_object(bool check_for_eos)
{
  if (mapper_runtime_.key.empty()) {
    base::on_parse_object(check_for_eos);
  } else {
    mapper_runtime_.object_key = mapper_runtime_.key;
    serializer_.serialize(object_);
    mapper_runtime_.object_key.clear();
  }
}

template<class T, class C>
void basic_json_mapper<T, C>::on_begin_object()
{
  if (is_array_) {
    is_object_ = true;
    object_ = serializer_.template create<T>();
  } else {
    mapper_runtime_.object_key = mapper_runtime_.key;
  }
  mapper_runtime_.key.clear();
}

template<class T, class C>
void basic_json_mapper<T, C>::on_object_key(const std::string &key)
{
  mapper_runtime_.key = key;
}

template<class T, class C>
void basic_json_mapper<T, C>::on_end_object()
{
  mapper_runtime_.object_key.clear();
  mapper_runtime_.key.clear();
  if (is_array_) {
    is_object_ = false;
    array_.emplace_back(std::move(object_));
  }
}

template<class T, class C>
void basic_json_mapper<T, C>::on_begin_array()
{
  is_array_ = true;
  mapper_runtime_.array_key = mapper_runtime_.key;
  mapper_runtime_.json_array_cursor = this->cursor()();
  if (!mapper_runtime_.object_key.empty()) {
    serializer_.serialize(object_);
    this->sync_cursor(this->cursor()() - 1);
  } else {
    mapper_runtime_.value = json::array();
  }
}

template<class T, class C>
void basic_json_mapper<T, C>::on_end_array()
{
  is_array_ = false;
  mapper_runtime_.array_key.clear();
  mapper_runtime_.json_array_cursor = this->cursor()();
  if (!mapper_runtime_.key.empty()) {
    serializer_.serialize(object_);
  }
}

template< class T, class V >
void try_push_back(std::vector<T> &, json &jval, const V &value)
{
  jval.push_back(value);
}

template< class T >
void try_push_back(std::vector<T> &vec, json&, const std::string &value, typename std::enable_if<std::is_convertible<T, std::string>::value>::type*)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &vec, json&, long long value, typename std::enable_if<std::is_integral<T>::value && !std::is_same<bool, T>::value>::type *)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &vec, json&, double value, typename std::enable_if<std::is_floating_point<T>::value>::type *)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &vec, json&, bool value, typename std::enable_if<std::is_same<T, bool>::value>::type *)
{
  vec.push_back(value);
}

template<class T>
void try_push_back(std::vector<T> &, json&, std::nullptr_t)
{
}

template < class T, class C >
void basic_json_mapper<T, C>::on_string(const std::string &value)
{
  if (mapper_runtime_.object_key.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, mapper_runtime_.value, value);
    } else /*if (is_array_ && is_object_)*/ {
      mapper_runtime_.value = value;
      serializer_.serialize(object_);
    }
  } else {
    mapper_runtime_.value = value;
    serializer_.serialize(object_);
  }
}

template < class T, class C >
void basic_json_mapper<T, C>::on_integer(long long value)
{
  if (mapper_runtime_.object_key.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, mapper_runtime_.value, value);
    } else /*if (is_array_ && is_object_)*/ {
      mapper_runtime_.value = value;
      serializer_.serialize(object_);
    }
  } else {
    mapper_runtime_.value = value;
    serializer_.serialize(object_);
  }
}

template < class T, class C >
void basic_json_mapper<T, C>::on_real(double value)
{
  if (mapper_runtime_.object_key.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, mapper_runtime_.value, value);
    } else /*if (is_array_ && is_object_)*/ {
      mapper_runtime_.value = value;
      serializer_.serialize(object_);
    }
  } else {
    mapper_runtime_.value = value;
    serializer_.serialize(object_);
  }
}

template < class T, class C >
void basic_json_mapper<T, C>::on_bool(bool value)
{
  if (mapper_runtime_.object_key.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, mapper_runtime_.value, value);
    } else /*if (is_array_ && is_object_)*/ {
      mapper_runtime_.value = value;
      serializer_.serialize(object_);
    }
  } else {
    mapper_runtime_.value = value;
    serializer_.serialize(object_);
  }
}

template < class T, class C >
void basic_json_mapper<T, C>::on_null()
{
  if (mapper_runtime_.object_key.empty()) {
    if (is_array_ && !is_object_) {
      try_push_back(array_, mapper_runtime_.value, nullptr);
    } else {
      mapper_runtime_.value = nullptr;
      serializer_.serialize(object_);
    }
  } else {
    mapper_runtime_.value = nullptr;
    serializer_.serialize(object_);
  }
}
/// @endcond
}

#endif //MATADOR_BASIC_JSON_MAPPER_HPP
