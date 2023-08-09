#ifndef MATADOR_IDENTIFIER_HPP
#define MATADOR_IDENTIFIER_HPP

#include <memory>
#include <string>
#include <typeindex>

namespace matador {

struct null_type_t {};

namespace detail {

enum class identifier_type : unsigned int {
  INTEGRAL_TYPE,
  STRING_TYPE,
  NULL_TYPE
};

template<typename Type, class Enabled = void>
struct identifier_type_traits;

template<typename Type>
struct identifier_type_traits<Type, typename std::enable_if<std::is_integral<Type>::value>::type> {
  static identifier_type type() { return identifier_type::INTEGRAL_TYPE; }
  static std::string type_string() { return "integral"; }
  static bool is_valid(Type value) { return value > 0; }
  static std::string to_string(Type value) { return std::to_string(value); }
};

template<typename Type>
struct identifier_type_traits<Type, typename std::enable_if<std::is_same<Type, std::string>::value>::type> {
  static identifier_type type() { return identifier_type::STRING_TYPE; }
  static std::string type_string() { return "string"; }
  static bool is_valid(const Type &value) { return !value.empty(); }
  static std::string to_string(Type value) { return value; }
};

template<typename Type>
struct identifier_type_traits<Type, typename std::enable_if<std::is_same<Type, null_type_t>::value>::type> {
  static identifier_type type() { return identifier_type::NULL_TYPE; }
  static std::string type_string() { return "null"; }
  static bool is_valid() { return false; }
  static std::string to_string() { return "null_pk"; }
};

}

class identifier_serializer
{
public:
  virtual ~identifier_serializer() = default;

  virtual void serialize(short &, long) = 0;
  virtual void serialize(int &, long) = 0;
  virtual void serialize(long &, long) = 0;
  virtual void serialize(long long &, long) = 0;
  virtual void serialize(unsigned short &, long) = 0;
  virtual void serialize(unsigned int &, long) = 0;
  virtual void serialize(unsigned long &, long) = 0;
  virtual void serialize(unsigned long long &, long) = 0;
  virtual void serialize(std::string &, long) = 0;
  virtual void serialize(null_type_t &, long) = 0;
};

class identifier
{
private:
  struct base
  {
    explicit base(const std::type_index &ti, detail::identifier_type id_type);
    base(const base &x) = delete;
    base &operator=(const base &x) = delete;
    base(base &&x) = delete;
    base &operator=(base &&x) = delete;
    virtual ~base() = default;

    template<typename Type>
    bool is_similar_type() const
    {
      return identifier_type_ == detail::identifier_type_traits<Type>::type();
    }

    bool is_similar_type(const base &x) const;
    detail::identifier_type type() const;

    virtual base *copy() const = 0;
    virtual bool equal_to(const base &x) const = 0;
    virtual bool less(const base &x) const = 0;
    virtual bool is_valid() const = 0;
    virtual void serialize(identifier_serializer &s) = 0;
    virtual std::string str() const = 0;
    virtual size_t hash() const = 0;

    std::type_index type_index_;
    detail::identifier_type identifier_type_;
  };

  template<class IdType>
  struct pk : public base
  {
    using self = pk<IdType>;

    explicit pk(const IdType &id, long size = -1) : base(std::type_index(typeid(IdType)), detail::identifier_type_traits<IdType>::type())
    , id_(id)
    , size_(size) {}

    base *copy() const final {
      return new self(id_, size_);
    }

    bool equal_to(const base &x) const final {
      return static_cast<const pk<IdType> &>(x).id_ == id_;
    }

    bool less(const base &x) const final {
      return static_cast<const pk<IdType> &>(x).id_ < id_;
    }

    bool is_valid() const final
    {
      return detail::identifier_type_traits<IdType>::is_valid(id_);
    }

    std::string str() const final
    {
      return detail::identifier_type_traits<IdType>::to_string(id_);
    }

    void serialize(identifier_serializer &s) final {
      s.serialize(id_, size_);
    }

    size_t hash() const final {
      std::hash<IdType> hash_func;
      return hash_func(id_);
    }

    IdType id_;
    long size_{-1};
  };

  struct null_pk : public base
  {
    null_pk();
    base *copy() const final;
    bool equal_to(const base &x) const final;
    bool less(const base &x) const final;
    bool is_valid() const final;
    void serialize(identifier_serializer &s) final;
    std::string str() const final;
    size_t hash() const final;
    null_type_t null_;
  };

public:
  identifier();
  template<typename Type>
  explicit identifier(const Type &id, long size = -1)
    : id_(std::make_shared<pk<Type>>(id, size)) {}
  identifier(const identifier &x);
  identifier &operator=(const identifier &x);
  identifier(identifier &&x) noexcept ;
  identifier &operator=(identifier &&x) noexcept;

  template<typename Type>
  identifier &operator=(const Type &value)
  {
    id_ = std::make_shared<pk<Type>>(value);
    return *this;
  }

  ~identifier() = default;

  bool operator==(const identifier &x) const;
  bool operator!=(const identifier &x) const;
  bool operator<(const identifier &x) const;
  bool operator<=(const identifier &x) const;
  bool operator>(const identifier &x) const;
  bool operator>=(const identifier &x) const;

//  bool is_same_type(const identifier &x) const;
  bool is_similar_type(const identifier &x) const;
  template<typename Type>
  bool is_similar_type() const
  {
    return id_->is_similar_type<Type>();
  }


  //  template<typename Type>
//  bool is_same_type() const;
  std::string str() const;
  const std::type_index &type_index() const;

  identifier share() const;
  size_t use_count() const;

  bool is_null() const;
  bool is_valid() const;
  void clear();

  void serialize(identifier_serializer &s);

  size_t hash() const;

  friend std::ostream &operator<<(std::ostream &out, const identifier &id);

private:
  explicit identifier(const std::shared_ptr<base>& id);

private:
  std::shared_ptr<base> id_;
};

static identifier null_identifier{};

//template<typename Type>
//bool identifier::is_same_type() const
//{
//  return id_->is_same_type(std::type_index(typeid(Type)));
//}

struct id_pk_hash
{
  size_t operator()(const identifier &id) const;
};

}

#endif //MATADOR_IDENTIFIER_HPP
