#include "matador/utils/json.hpp"
#include "matador/utils/string.hpp"

#include <sstream>
#include <iostream>
#include <array>

namespace matador {

json_format json_format::compact { false, true, 0 }; /* NOLINT */
json_format json_format::pretty { true, false, 2 }; /* NOLINT */

inline json_format::json_format(bool enable_line_break)
  : enable_line_break_(enable_line_break)
{}

inline json_format::json_format(unsigned indentation)
  : indentation_(indentation)
{}

inline json_format::json_format(bool enable_line_break, bool skip_empty, unsigned indentation)
  : enable_line_break_(enable_line_break)
  , skip_empty_(skip_empty)
  , indentation_(indentation)
{}

bool json_format::show_line_break() const
{
  return enable_line_break_;
}

bool json_format::skip_empty()
{
  return skip_empty_;
}

unsigned json_format::indentation() const
{
  return indentation_;
}

char json_format::indentation_char() const
{
  return indentation_char_;
}

json::json() : value_(), type_(e_null) {}

json::json(bool b)
  : value_(b), type_(e_boolean)
{}

json::json(std::initializer_list<json> l)
  : type_(e_array)
{
  bool is_obj = std::all_of(l.begin(), l.end(), [](const json &j) {
    return j.is_array() && j.size() == 2 && j[0].is_string();
  });
  if (is_obj) {
    auto *obj = new object_type;
    for (auto &i : l) {
      obj->insert(std::make_pair(i.value_.array->at(0).as<std::string>(), i[1]));
    }
    clear();
    type_ = e_object;
    value_.object = obj;
  } else {
    value_.array = new array_type(l);
  }
}

json::json(std::nullptr_t) : type_(e_null)
{

}

json::json(json::json_type t) : value_(), type_(t)
{
  switch (type_) {
    case e_string:
      value_.str = new std::string();
      break;
    case e_object:
      value_.object = new object_type;
      break;
    case e_array:
      value_.array = new array_type;
      break;
    default:
      break;
  }
}

json::~json()
{
  clear();
}

json json::object()
{
  return json(e_object);
}

json json::array()
{
  return json(e_array);
}

json::json(const json &x)
{
  copy_from(x);
}

json::json(json &&x) noexcept
{
  value_ = x.value_;
  type_ = x.type_;
  x.type_ = e_null;
}

json &json::operator=(json &&x) noexcept
{
  clear();
  value_ = x.value_;
  type_ = x.type_;
  x.type_ = e_null;
  return *this;
}

json &json::operator=(const json &x)
{
  if (this == &x) {
    return *this;
  }
  clear();
  copy_from(x);
  return *this;
}

bool operator==(const json &a, const json &b)
{
  if (a.type_ != b.type_) {
    return false;
  }
  switch (a.type_) {
    case json::e_object:
      return *a.value_.object == *b.value_.object;
    case json::e_array:
      return *a.value_.array == *b.value_.array;
    case json::e_null:
      return true;
    case json::e_boolean:
      return a.value_.boolean == b.value_.boolean;
    case json::e_string:
      return *a.value_.str == *b.value_.str;
    case json::e_real:
      return a.value_.real == b.value_.real;
    case json::e_integer:
      return a.value_.integer == b.value_.integer;
  }
  return false;
}

bool operator!=(const json &a, const json &b)
{
  return !operator==(a, b);
}

bool operator<(json::json_type a, json::json_type b);

bool operator<(const json &a, const json &b)
{
  if (a.type_ == b.type_) {
    switch (a.type_) {
      case json::e_object:
        return (*a.value_.object) < (*b.value_.object);
      case json::e_array:
        return (*a.value_.array) < (*b.value_.array);
      case json::e_null:
        return false;
      case json::e_boolean:
        return (a.value_.boolean) < (b.value_.boolean);
      case json::e_string:
        return (*a.value_.str) < (*b.value_.str);
      case json::e_real:
        return (a.value_.real) < (b.value_.real);
      case json::e_integer:
        return (a.value_.integer) < (b.value_.integer);
    }
  } else if (a.type_ == json::e_integer && b.type_ == json::e_real) {
    return static_cast<double>(a.value_.integer) < b.value_.real;
  } else if (a.type_ == json::e_real && b.type_ == json::e_integer) {
    return a.value_.real < static_cast<double>(b.value_.integer);
  }

  return operator<(a.type_, b.type_);
}

/*
 * When ordering by json type the following ordering is used
 * null < boolean < number < object < array < string
 */
bool operator<(json::json_type a, json::json_type b)
{
  static constexpr std::array<unsigned char, 7> order = {{
    0 /* null */,
    2 /* integer */,
    2 /* real */,
    1 /* boolean */,
    5 /* string */,
    3 /* object */,
    4 /* array */
  }};

  return (size_t)a < order.size() && (size_t)b < order.size() && order[a] < order[b];
}

bool operator<=(const json &a, const json &b)
{
  return !(b < a);
}

bool operator>(const json &a, const json &b)
{
  return !(a <= b);
}

bool operator>=(const json &a, const json &b)
{
  return !(a < b);
}

std::ostream &operator<<(std::ostream &out, const json &val)
{
  val.dump(out);
  return out;
}

void json::dump(std::ostream &out) const
{
  switch (type_) {
    case e_object:
      out << "{";
      join(*value_.object, out, ", ");
      out << "}";
      break;
    case e_array:
      out << "[";
      join(*value_.array, out, ", ");
      out << "]";
      break;
    case e_null:
      out << "null";
      break;
    case e_integer:
      out << value_.integer;
      break;
    case e_real:
      out << value_.real;
      break;
    case e_string:
      out << "\"" << *value_.str << "\"";
      break;
    case e_boolean:
      out << std::boolalpha << value_.boolean;
      break;
    default:
      break;
  }
}

void json::copy_from(const json &x)
{
  type_ = x.type_;
  switch (x.type_) {
    case e_object:
      value_.object = new object_type(*x.value_.object);
      break;
    case e_array:
      value_.array = new array_type(*x.value_.array);
      break;
    case e_string:
      value_.str = new std::string(*x.value_.str);
      break;
    default:
      value_ = x.value_;
  }
}

bool json::contains(const std::string &key) const
{
  if (!is_object()) {
    return false;
  }
  return value_.object->find(key) != value_.object->end();
}

json &json::operator[](std::size_t i)
{
  if (type_ != e_array) {
    return *this;
  }
  if (i >= value_.array->size()) {
    throw std::logic_error("index out of bounds");
  }
  return value_.array->at(i);
}

void json::reset(bool b)
{
  if (type_ != e_boolean) {
    clear();
    type_ = e_boolean;
  }
  value_.boolean = b;
}

std::size_t json::size() const
{
  switch (type_) {
    case e_array:
      return value_.array->size();
    case e_object:
      return value_.object->size();
    case e_null:
      return 0;
    default:
      return 1;
  }
}

bool json::empty() const
{
  switch (type_) {
    case e_array:
      return value_.array->empty();
    case e_object:
      return value_.object->empty();
    case e_null:
      return true;
    default:
      return false;
  }
}

json &json::back()
{
  if (type_ == e_array) {
    return value_.array->back();
  } else {
    throw std::logic_error("type doesn't have back()");
  }
}

const json &json::back() const
{
  if (type_ == e_array) {
    return value_.array->back();
  } else {
    throw std::logic_error("type doesn't have back()");
  }
}

void json::erase(const std::string &key)
{
  if (type_ != e_object) {
    return;
  }
  value_.object->erase(key);
}

void json::erase(std::size_t i)
{
  if (type_ != e_array) {
    return;
  }
  if (i >= value_.array->size()) {
    throw std::out_of_range("index is out of range");
  }
  value_.array->erase(value_.array->begin()+i);
}

json::json_type json::type() const
{
  return type_;
}

bool json::is_number() const
{
  return type_ == e_integer || type_ == e_real;
}

bool json::is_real() const
{
  return type_ == e_real;
}

bool json::is_integer() const
{
  return type_ == e_integer;
}

bool json::is_boolean() const
{
  return type_ == e_boolean;
}

bool json::is_string() const
{
  return type_ == e_string;
}

bool json::is_array() const
{
  return type_ == e_array;
}

bool json::is_object() const
{
  return type_ == e_object;
}

bool json::is_null() const
{
  return type_ == e_null;
}

json &json::get(const std::string &key)
{
  if (type_ != e_object) {
    throw std::logic_error("type isn't object");
  }

  auto it = value_.object->find(key);
  if (it == value_.object->end()) {
    throw std::logic_error("object doesn't contain key " + key);
  } else {
    return value_.object->at(key);
    //return it->second;
  }
}

const json &json::get(const std::string &key) const
{
  if (type_ != e_object) {
    throw std::logic_error("type isn't object");
  }

  auto it = value_.object->find(key);
  if (it == value_.object->end()) {
    throw std::logic_error("object doesn't contain key " + key);
  } else {
    return it->second;
  }
}

json& func(json &parent, const std::vector<std::string> &parts, size_t index);

json& json::at_path(const std::string &path, char delimiter)
{
  std::vector<std::string> parts;
  matador::split(path, delimiter, parts);

  if (parts.empty()) {
    return *this;
  }

  return func((*this).get(parts[0]), parts, 1);

//  json& result = (*this)[parts[0]];
//
//  for (size_t i = 1; i < parts.size(); ++i) {
//    if (!result.is_object()) {
//      throw std::logic_error("couldn't find json at path " + path);
//    }
//    result = result.get(parts[i]);
//  }
//
//  return result;
}

json& func(json &parent, const std::vector<std::string> &parts, size_t index)
{
  if (index < parts.size()) {
    return func(parent.get(parts[index]), parts, index+1);
  } else {
    return parent;
  }
}

const json& json::at_path(const std::string &path, char delimiter) const
{
  std::vector<std::string> parts;
  matador::split(path, delimiter, parts);

  if (parts.empty()) {
    return *this;
  }

  return func(const_cast<json&>(*this).get(parts[0]), parts, 1);
}

void json::throw_on_wrong_type(json::json_type t) const
{
  if (type_ != t) {
    throw std::logic_error("wrong type; couldn't cast");
  }
}

json::iterator json::begin()
{
  return iterator(this, true);
}

json::const_iterator json::begin() const
{
  return const_iterator(this, true);
}

json::iterator json::end()
{
  return iterator(this, false);
}

json::const_iterator json::end() const
{
  return const_iterator(this, false);
}

json &json::operator[](const std::string &key)
{
  if (type_ != e_object) {
    clear();
    value_.object = new object_type;
    type_ = e_object;
  }
  auto it = value_.object->insert(std::make_pair(key, json())).first;
  return it->second;
}

const json &json::operator[](const std::string &key) const
{
  if (type_ != e_object) {
    return *this;
  }

  auto it = value_.object->find(key);
  if (it == value_.object->end()) {
    return *this;
  } else {
    return it->second;
  }
}

const json &json::operator[](std::size_t i) const
{
  if (type_ != e_array) {
    return *this;
  }
  if (i >= value_.array->size()) {
    throw std::logic_error("index out of bounce");
  }
  return value_.array->at(i);
}

std::string json::str() const
{
  return str(json_format::compact);
}

std::string json::str(const json_format &format) const
{
  std::string result {};
  switch (type_) {
    case json::json_type::e_object:
      result += "{";
      if (format.show_line_break()) {
        result += "\n";
      }
      result += join(*value_.object, ", ");
      result += "}";
      break;
    case json::json_type::e_array:
      result += "[";
      result += join(*value_.array, ", ");
      result += "]";
      break;
    case json::json_type::e_null:
      result += "null";
      break;
    case json::json_type::e_integer:
      result += std::to_string(value_.integer);
      break;
    case json::json_type::e_real:
      result += std::to_string(value_.real);
      break;
    case json::json_type::e_string:
      result += "\"" + *value_.str + "\"";
      break;
    case json::json_type::e_boolean:
      result += (value_.boolean ? "true" : "false");
      break;
    default:
      break;
  }
  return result;
}

std::string to_string(const json &j)
{
  return j.str();
}
}