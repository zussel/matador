#include <iostream>
#include <string>
#include <typeindex>

namespace test {

enum class identifier_type : unsigned int {
  INTEGRAL_TYPE,
  FLOATING_POINT_TYPE,
  STRING_TYPE,
  NULL_TYPE
};

struct null_type;

template < typename Type, class Enabled = void >
struct identifier_type_traits;

template < typename Type >
struct identifier_type_traits<Type, typename std::enable_if<std::is_integral<Type>::value>::type>
{
  static identifier_type type() { return identifier_type::INTEGRAL_TYPE; }
  static std::string type_string() { return "integral"; }
  static bool is_valid(Type value) { return value > 0; }
  static std::string to_string(Type value) { return std::to_string(value); }
};

template < typename Type >
struct identifier_type_traits<Type, typename std::enable_if<std::is_floating_point<Type>::value>::type>
{
  static identifier_type type() { return identifier_type::FLOATING_POINT_TYPE; }
  static std::string type_string() { return "floating_point"; }
  static bool is_valid(Type value) { return value > 0.0; }
  static std::string to_string(Type value) { return std::to_string(value); }
};

template < typename Type >
struct identifier_type_traits<Type, typename std::enable_if<std::is_same<Type, std::string>::value>::type>
{
  static identifier_type type() { return identifier_type::STRING_TYPE; }
  static std::string type_string() { return "string"; }
  static bool is_valid(const Type& value) { return !value.empty(); }
  static std::string to_string(Type value) { return value; }
};

template < typename Type >
struct identifier_type_traits<Type, typename std::enable_if<std::is_same<Type, null_type>::value>::type>
{
  static identifier_type type() { return identifier_type::NULL_TYPE; }
  static std::string type_string() { return "null"; }
  static bool is_valid(Type value) { return true; }
  static std::string to_string(Type value) { return "null"; }
};

class identifier_serializer;

struct base
{
  base(const std::type_index &ti, identifier_type id_type) : type_index_(ti), identifier_type_(id_type) {}
  base(const base &x) = delete;
  base &operator=(const base &x) = delete;
  base(base &&x) = delete;
  base &operator=(base &&x) = delete;
  virtual ~base() = default;

  bool is_same_type(const base &x) const { return is_same_type(x.type_index_); }
  bool is_same_type(const std::type_index &ti) const { return type_index_ == ti; }

  bool is_similar_type(const base &x) const { return identifier_type_ == x.type(); }
  identifier_type type() const { return identifier_type_; }
  virtual base *copy() const = 0;
  virtual bool equal_to(const base &x) const = 0;
  virtual bool less(const base &x) const = 0;
  virtual bool is_valid() const = 0;
  virtual void serialize(identifier_serializer &s) = 0;
  virtual std::string str() const = 0;
  virtual size_t hash() const = 0;

  std::type_index type_index_;
  identifier_type identifier_type_;
};

template<class IdType>
struct pk : public base
{
  using self = pk<IdType>;

  explicit pk(const IdType &id, long size = -1)
  : base(std::type_index(typeid(IdType)), identifier_type_traits<IdType>::type()), id_(id), size_(size) {}

  base *copy() const final {
    return new self(id_, size_);
  }

  bool equal_to(const base &x) const final {
    return static_cast<const pk<IdType> &>(x).id_ == id_;
  }

  bool less(const base &x) const final {
    return static_cast<const pk<IdType> &>(x).id_ < id_;
  }

  bool is_valid() const final {
    return identifier_type_traits<IdType>::is_valid(id_);
  }

  std::string str() const final {
    return identifier_type_traits<IdType>::to_string(id_);
  }

  void serialize(identifier_serializer &s) final {
//    s.serialize(id_, size_);
  }

  size_t hash() const final {
    std::hash<IdType> hash_func;
    return hash_func(id_);
  }

  IdType id_;
  long size_{-1};
};

}

int main(int /*argc*/, char* /*argv*/[]) {

  test::pk<long> lid(7);
  test::pk<int> iid(8);
  test::pk<int> iid2(7);

  auto b = lid.is_similar_type(iid);
  b = lid.is_same_type(iid);

  b = lid.equal_to(iid);
  b = lid.equal_to(iid2);
}