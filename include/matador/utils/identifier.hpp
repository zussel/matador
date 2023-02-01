#ifndef MATADOR_IDENTIFIER_HPP
#define MATADOR_IDENTIFIER_HPP

#include <memory>
#include <string>
#include <typeindex>

namespace matador {
namespace detail {
template<typename T>
std::string to_string(const T &value) {
  return std::to_string(value);
}

std::string to_string(const std::string &value);

template<typename T>
bool is_valid(const T &value) {
  return value > 0;
}

bool is_valid(const std::string &value);
}

struct null_type_t {};

class identifier_serializer
{
public:
  virtual ~identifier_serializer() = default;

  virtual void serialize(short &) = 0;
  virtual void serialize(int &) = 0;
  virtual void serialize(long &) = 0;
  virtual void serialize(long long &) = 0;
  virtual void serialize(unsigned short &) = 0;
  virtual void serialize(unsigned int &) = 0;
  virtual void serialize(unsigned long &) = 0;
  virtual void serialize(unsigned long long &) = 0;
  virtual void serialize(std::string &) = 0;
  virtual void serialize(null_type_t &) = 0;
};

class identifier
{
private:
  struct base
  {
    explicit base(const std::type_index &ti);
    base(const base &x) = delete;
    base &operator=(const base &x) = delete;
    base(base &&x) = delete;
    base &operator=(base &&x) = delete;
    virtual ~base() = default;

    bool is_same_type(const base &x) const;
    bool is_same_type(const std::type_index &x) const;

    virtual base *copy() const = 0;
    virtual bool equal_to(const base &x) const = 0;
    virtual bool less(const base &x) const = 0;
    virtual bool is_valid() const = 0;
    virtual void serialize(identifier_serializer &s) = 0;
    virtual std::string str() const = 0;
    virtual size_t hash() const = 0;

    std::type_index type_index_;
  };

  template<class IdType>
  struct pk : public base
  {
    using self = pk<IdType>;

    explicit pk(IdType id)
      : base(std::type_index(typeid(IdType))), id_(std::move(id)) {}

    base *copy() const final {
      return new self(id_);
    }

    bool equal_to(const base &x) const final {
      return is_same_type(x) && static_cast<const pk<IdType> &>(x).id_ == id_;
    }

    bool less(const base &x) const final {
      return is_same_type(x) && static_cast<const pk<IdType> &>(x).id_ < id_;
    }

    bool is_valid() const final {
      return detail::is_valid(id_);
    }

    std::string str() const final {
      return detail::to_string(id_);
    }

    void serialize(identifier_serializer &s) final {
      s.serialize(id_);
    }

    size_t hash() const final {
      std::hash<IdType> hash_func;
      return hash_func(id_);
    }

    IdType id_;
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
  explicit identifier(const Type &id)
    : id_(std::make_shared<pk<Type>>(id)) {}
  identifier(const identifier &x);
  identifier &operator=(const identifier &x);
  identifier(identifier &&x) noexcept ;
  identifier &operator=(identifier &&x) noexcept;

  template<typename Type>
  identifier &operator=(const Type &value) {
    id_ = std::make_unique<pk<Type>>(value);
    return *this;
  }

  ~identifier() = default;

  bool operator==(const identifier &x) const;
  bool operator!=(const identifier &x) const;
  bool operator<(const identifier &x) const;
  bool operator<=(const identifier &x) const;
  bool operator>(const identifier &x) const;
  bool operator>=(const identifier &x) const;

  bool is_same_type(const identifier &x) const;

  template<typename Type>
  bool is_same_type() const;
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

template<typename Type>
bool identifier::is_same_type() const
{
  return id_->is_same_type(std::type_index(typeid(Type)));
}

struct id_pk_hash
{
  size_t operator()(const identifier &id) const;
};

}

#endif //MATADOR_IDENTIFIER_HPP
