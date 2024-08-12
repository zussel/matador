#ifndef MATADOR_IDENTIFIER_HPP
#define MATADOR_IDENTIFIER_HPP

#include "matador/utils/field_attributes.hpp"
#include "matador/utils/data_types.hpp"

#include "data_type_traits.hpp"

#include <memory>
#include <string>
#include <typeindex>

namespace matador {

/// @cond MATADOR_DEV
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
struct identifier_type_traits<Type, typename std::enable_if<std::is_same<Type, utils::null_type_t>::value>::type> {
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

  virtual void serialize(short &, const utils::field_attributes &) = 0;
  virtual void serialize(int &, const utils::field_attributes &) = 0;
  virtual void serialize(long &, const utils::field_attributes &) = 0;
  virtual void serialize(long long &, const utils::field_attributes &) = 0;
  virtual void serialize(unsigned short &, const utils::field_attributes &) = 0;
  virtual void serialize(unsigned int &, const utils::field_attributes &) = 0;
  virtual void serialize(unsigned long &, const utils::field_attributes &) = 0;
  virtual void serialize(unsigned long long &, const utils::field_attributes &) = 0;
  virtual void serialize(std::string &, const utils::field_attributes &) = 0;
  virtual void serialize(utils::null_type_t &, const utils::field_attributes &) = 0;
};

/// @endcond

class identifier
{
private:
  struct base
  {
    explicit base(const std::type_index &ti, detail::identifier_type id_type, data_type type);
    base(const base &x) = delete;
    base &operator=(const base &x) = delete;
    base(base &&x) = delete;
    base &operator=(base &&x) = delete;
    virtual ~base() = default;

    template<typename Type>
    [[nodiscard]] bool is_similar_type() const
    {
      return identifier_type_ == detail::identifier_type_traits<Type>::type();
    }

    [[nodiscard]] bool is_similar_type(const base &x) const;
    [[nodiscard]] detail::identifier_type type() const;

    [[nodiscard]] virtual base *copy() const = 0;
    [[nodiscard]] virtual bool equal_to(const base &x) const = 0;
    [[nodiscard]] virtual bool less(const base &x) const = 0;
    [[nodiscard]] virtual bool is_valid() const = 0;
    virtual void serialize(identifier_serializer &s) = 0;
    [[nodiscard]] virtual std::string str() const = 0;
    [[nodiscard]] virtual size_t hash() const = 0;

    std::type_index type_index_;
    detail::identifier_type identifier_type_;
    data_type type_{data_type::type_unknown};
  };

  template<class IdType>
  struct pk : public base
  {
    using self = pk<IdType>;

    explicit pk(const IdType &id, size_t size = 0)
    : base(std::type_index(typeid(IdType)), detail::identifier_type_traits<IdType>::type(),
           object::data_type_traits<IdType>::type(size))
    , id_(id)
    , size_(size) {}

    [[nodiscard]] base *copy() const final {
      return new self(id_, size_);
    }

    [[nodiscard]] bool equal_to(const base &x) const final {
      return static_cast<const pk<IdType> &>(x).id_ == id_;
    }

    [[nodiscard]] bool less(const base &x) const final {
      return static_cast<const pk<IdType> &>(x).id_ < id_;
    }

    [[nodiscard]] bool is_valid() const final
    {
      return detail::identifier_type_traits<IdType>::is_valid(id_);
    }

    [[nodiscard]] std::string str() const final
    {
      return detail::identifier_type_traits<IdType>::to_string(id_);
    }

    void serialize(identifier_serializer &s) final {
      s.serialize(id_, size_);
    }

    [[nodiscard]] size_t hash() const final {
      std::hash<IdType> hash_func;
      return hash_func(id_);
    }

    IdType id_;
    size_t size_{};
  };

  struct null_pk : public base
  {
    null_pk();
    [[nodiscard]] base *copy() const final;
    [[nodiscard]] bool equal_to(const base &x) const final;
    [[nodiscard]] bool less(const base &x) const final;
    [[nodiscard]] bool is_valid() const final;
    void serialize(identifier_serializer &s) final;
    [[nodiscard]] std::string str() const final;
    [[nodiscard]] size_t hash() const final;
    utils::null_type_t null_;
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

  [[nodiscard]] bool is_similar_type(const identifier &x) const;
  template<typename Type>
  [[nodiscard]] bool is_similar_type() const
  {
    return id_->is_similar_type<Type>();
  }

  [[nodiscard]] std::string str() const;
  [[nodiscard]] const std::type_index &type_index() const;
  [[nodiscard]] data_type type() const;

  [[nodiscard]] identifier share() const;
  [[nodiscard]] size_t use_count() const;

  [[nodiscard]] bool is_null() const;
  [[nodiscard]] bool is_valid() const;
  void clear();

  void serialize(identifier_serializer &s);

  [[nodiscard]] size_t hash() const;

  friend std::ostream &operator<<(std::ostream &out, const identifier &id);

private:
  explicit identifier(const std::shared_ptr<base>& id);

private:
  std::shared_ptr<base> id_;
};

static identifier null_identifier{};

/// @cond MATADOR_DEV
struct id_pk_hash
{
  size_t operator()(const identifier &id) const;
};
/// @endcond
}

#endif //MATADOR_IDENTIFIER_HPP
