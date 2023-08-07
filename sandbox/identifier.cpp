#include <iostream>
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
  static std::string str() { return "integral"; }
  static bool is_valid(Type value) { return value > 0; }
};

template < typename Type >
struct identifier_type_traits<Type, typename std::enable_if<std::is_floating_point<Type>::value>::type>
{
  static identifier_type type() { return identifier_type::FLOATING_POINT_TYPE; }
  static std::string str() { return "floating_point"; }
  static bool is_valid(Type value) { return value > 0.0; }
};

template < typename Type >
struct identifier_type_traits<Type, typename std::enable_if<std::is_same<Type, std::string>::value>::type>
{
  static identifier_type type() { return identifier_type::STRING_TYPE; }
  static std::string str() { return "string"; }
  static bool is_valid(const Type& value) { return !value.empty(); }
};

template < typename Type >
struct identifier_type_traits<Type, typename std::enable_if<std::is_same<Type, null_type>::value>::type>
{
  static identifier_type type() { return identifier_type::NULL_TYPE; }
  static std::string str() { return "null"; }
  static bool is_valid(Type value) { return true; }
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

  bool is_same_type(const base &x) const;
  bool is_same_type(const std::type_index &x) const;

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
    return detail::is_valid(id_);
  }

  std::string str() const final {
    return detail::to_string(id_);
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