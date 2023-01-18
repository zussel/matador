#ifndef MATADOR_ID_PK_HPP
#define MATADOR_ID_PK_HPP

#include <memory>
#include <string>
#include <typeindex>

namespace matador {
namespace detail {
template<typename T>
static std::string to_string(const T &value) {
  return std::to_string(value);
}

static std::string to_string(const std::string &value) {
  return value;
}
}

class id_pk
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

    virtual base *copy() const = 0;
    virtual bool equal_to(const base &x) const = 0;
    virtual bool less(const base &x) const = 0;
    virtual std::string str() const = 0;

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

    std::string str() const final {
      return detail::to_string(id_);
    }

    IdType id_;
  };

  struct null_pk : public base
  {
    struct null_type_t {};

    null_pk();
    base *copy() const final;
    bool equal_to(const base &x) const final;
    bool less(const base &x) const final;
    std::string str() const final;
  };

public:
  id_pk();
  template<typename Type>
  explicit id_pk(const Type &id)
    : id_(std::make_unique<pk<Type>>(id)) {}
  id_pk(const id_pk &x);
  id_pk &operator=(const id_pk &x);
  id_pk(id_pk &x);
  id_pk &operator=(id_pk &&x) noexcept;

  template<typename Type>
  id_pk &operator=(const Type &value) {
    id_ = std::make_unique<pk<Type>>(value);
    return *this;
  }

  ~id_pk() = default;

  bool operator==(const id_pk &x) const;
  bool operator!=(const id_pk &x) const;
  bool operator<(const id_pk &x) const;
  bool operator<=(const id_pk &x) const;
  bool operator>(const id_pk &x) const;
  bool operator>=(const id_pk &x) const;

  bool is_same_type(const id_pk &x) const;
  std::string str() const;
  const std::type_index &type_index() const;

  friend std::ostream &operator<<(std::ostream &out, const id_pk &id);

private:
  std::unique_ptr <base> id_;
};

static id_pk null_pk{};

}

#endif //MATADOR_ID_PK_HPP
