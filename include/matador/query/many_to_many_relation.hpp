#ifndef QUERY_HAS_MANY_TO_MANY_RELATION_HPP
#define QUERY_HAS_MANY_TO_MANY_RELATION_HPP

#include "matador/query/object_ptr.hpp"
#include "matador/query/access.hpp"
#include "matador/query/foreign_options.hpp"

namespace matador::query {
template < class ForeignType >
class generic_relation {
public:
    generic_relation() = default;
    generic_relation(std::string local_name, std::string remote_name)
    : local_name_(std::move(local_name))
    , remote_name_(std::move(remote_name)) {}
    template<typename LocalType>
    generic_relation(std::string local_name, std::string remote_name, const object_ptr<LocalType>& local, const ForeignType& remote)
    : local_name_(std::move(local_name))
    , remote_name_(std::move(remote_name))
    , local_(local)
    , local_type_(typeid(LocalType))
    , remote_(remote)
    {}

    template<class Operator>
    void process(Operator &op) {
        field::belongs_to(op, local_name_.c_str(), local_, CascadeNoneFetchLazy);
        foreign_field(op);
    }

    template<typename LocalType>
    object_ptr<LocalType> local() const {
        return local_;
    }
    template<typename LocalType>
    void local(const object_ptr<LocalType>& local) {
        local_ = local;
    }

    const ForeignType& remote() const { return remote_; }
    void remote(const ForeignType& remote) { remote_ = remote; }

    [[nodiscard]] const std::string& local_name() const { return local_name_; }
    [[nodiscard]] const std::string& remote_name() const { return remote_name_; }

private:
  template<typename Operator, typename  RemoteType = ForeignType>
  void foreign_field(Operator &op, std::enable_if_t<!is_object_ptr<RemoteType>::value>* /*unused*/) {
        field::attribute(op, remote_name_.c_str(), remote_);
  }
  template<typename Operator, class RemoteType = ForeignType>
  void foreign_field(Operator &op, std::enable_if_t<is_object_ptr<RemoteType>::value>* /*unused*/) {
        field::belongs_to(op, remote_name_.c_str(), remote_, CascadeNoneFetchLazy);
  }
private:
    std::string local_name_;
    std::string remote_name_;
    std::type_index local_type_{typeid(void)};
    void *local_{nullptr};
    ForeignType remote_;
};

template < class LocalType, class ForeignType >
class relation {
public:
    relation() = default;
    relation(std::string local_name, std::string remote_name)
    : local_name_(std::move(local_name))
    , remote_name_(std::move(remote_name)) {}
    relation(std::string local_name, std::string remote_name, const object_ptr<LocalType>& local, const ForeignType& remote)
    : local_name_(std::move(local_name))
    , remote_name_(std::move(remote_name))
    , local_(local)
    , remote_(remote)
    {}

    template<class Operator>
    void process(Operator &op) {
        field::belongs_to(op, local_name_.c_str(), local_, CascadeNoneFetchLazy);
        foreign_field(op);
    }

    object_ptr<LocalType> local() const { return local_; }
    void local(const object_ptr<LocalType>& local) { local_ = local; }
    const ForeignType& remote() const { return remote_; }
    void remote(const ForeignType& remote) { remote_ = remote; }

    [[nodiscard]] const std::string& local_name() const { return local_name_; }
    [[nodiscard]] const std::string& remote_name() const { return remote_name_; }

private:
  template<typename Operator, class RemoteType = ForeignType>
  void foreign_field(Operator &op, std::enable_if_t<!is_object_ptr<RemoteType>::value>* /*unused*/) {
        field::attribute(op, remote_name_.c_str(), remote_);
  }
  template<typename Operator, class RemoteType = ForeignType>
  void foreign_field(Operator &op, std::enable_if_t<is_object_ptr<RemoteType>::value>* /*unused*/) {
        field::belongs_to(op, remote_name_.c_str(), remote_, CascadeNoneFetchLazy);
  }
private:
    std::string local_name_;
    std::string remote_name_;
    object_ptr<LocalType> local_;
    ForeignType remote_;
};

template < class LocalType, class ForeignType >
class many_to_many_relation {
public:
    many_to_many_relation() = default;
    many_to_many_relation(std::string local_name, std::string remote_name)
    : local_name_(std::move(local_name))
    , remote_name_(std::move(remote_name)) {}
    many_to_many_relation(std::string local_name, std::string remote_name, const object_ptr<LocalType>& local, const object_ptr<ForeignType>& remote)
    : local_name_(std::move(local_name))
    , remote_name_(std::move(remote_name))
    , local_(local)
    , remote_(remote)
    {}

    template<class Operator>
    void process(Operator &op) {
        field::belongs_to(op, local_name_.c_str(), local_, CascadeNoneFetchLazy);
        field::belongs_to(op, remote_name_.c_str(), remote_, CascadeNoneFetchLazy);
    }

    object_ptr<LocalType> local() const { return local_; }
    object_ptr<LocalType> remote() const { return remote_; }

private:
    std::string local_name_;
    std::string remote_name_;
    object_ptr<LocalType> local_;
    object_ptr<ForeignType> remote_;
};

template < class LocalType, class Type >
class many_to_relation {
public:
    many_to_relation() = default;
    many_to_relation(std::string local_name, std::string remote_name)
    : local_name_(std::move(local_name))
    , type_name_(std::move(remote_name)) {}
    many_to_relation(std::string local_name, std::string remote_name, const object_ptr<LocalType>& local, const Type& value)
    : local_name_(std::move(local_name))
    , type_name_(std::move(remote_name))
    , local_(local)
    , value_(value){}

    template<class Operator>
    void process(Operator &op) {
        field::belongs_to(op, local_name_.c_str(), local_, CascadeNoneFetchLazy);
        field::attribute(op, type_name_.c_str(), value_);
    }

    object_ptr<LocalType> local() const { return local_; }
    Type value() const { return value_; }

private:
    std::string local_name_;
    std::string type_name_;
    object_ptr<LocalType> local_;
    Type value_;
};

}
#endif //QUERY_HAS_MANY_TO_MANY_RELATION_HPP
