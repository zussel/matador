#ifndef QUERY_QUERY_RESULT_HPP
#define QUERY_QUERY_RESULT_HPP

#include "matador/query/column.hpp"
#include "matador/query/error_code.hpp"
#include "matador/query/internal/query_result_impl.hpp"
#include "matador/query/object_ptr.hpp"
#include "matador/query/record.hpp"
#include "matador/query/field.hpp"

#include "matador/utils/error.hpp"
#include "matador/utils/result.hpp"

#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace matador::query {
class query_result_impl;

template <typename Type>
class query_result;

template <typename Type>
class query_result_iterator;

namespace detail {

template <typename Type>
struct query_result_storage {
  using creator_func = std::function<std::shared_ptr<Type>()>;

  std::shared_ptr<object_resolver<Type>> resolver;
  creator_func creator;
};

template <>
struct query_result_storage<record> {
  std::vector<column> prototype;
};

template <typename Type>
struct query_result_traits {
  using stored_type = object_ptr<Type>;
  using dereference_type = object_ptr<Type>;
  using result_type = query_result<Type>;
  using storage_type = query_result_storage<Type>;
  using creator_func = typename storage_type::creator_func;

  [[nodiscard]] static result<result_type, error> make_query_result(
    std::unique_ptr<query_result_impl>&& impl,
    std::shared_ptr<object_resolver<Type>> resolver,
    creator_func&& creator);

  static void clear(stored_type& value);
  static dereference_type dereference(stored_type& value);
  static bool read_next(query_result<Type>& result, stored_type& value);
};

template <>
struct query_result_traits<record> {
  using stored_type = record;
  using dereference_type = record&;
  using result_type = query_result<record>;
  using storage_type = query_result_storage<record>;

  [[nodiscard]] static result<result_type, error> make_query_result(
    std::unique_ptr<query_result_impl>&& impl,
    std::vector<column> prototype);

  static void clear(stored_type& value);
  static dereference_type dereference(stored_type& value);
  static bool read_next(query_result<record>& result, stored_type& value);
};

}

template <typename Type>
class query_result_iterator {
public:
  using traits = detail::query_result_traits<Type>;
  using iterator_category = std::input_iterator_tag;
  using value_type = typename traits::stored_type;
  using difference_type = std::ptrdiff_t;
  using self = query_result_iterator;
  using pointer = value_type*;
  using reference = typename traits::dereference_type;

public:
  query_result_iterator() = default;

  explicit query_result_iterator(query_result<Type>* res)
  : result_(res)
  , at_end_(res == nullptr) {}

  query_result_iterator(query_result_iterator&&) noexcept = default;
  query_result_iterator& operator=(query_result_iterator&&) noexcept = default;

  query_result_iterator(const query_result_iterator&) = default;
  query_result_iterator& operator=(const query_result_iterator&) = default;

  ~query_result_iterator() = default;

  bool operator==(const query_result_iterator& rhs) const {
    if (at_end_ && rhs.at_end_) {
      return true;
    }

    return result_ == rhs.result_ && at_end_ == rhs.at_end_;
  }

  bool operator!=(const query_result_iterator& rhs) const {
    return !(*this == rhs);
  }

  self& operator++() {
    read_next();
    return *this;
  }

  self operator++(int) {
    self tmp(*this);
    read_next();
    return tmp;
  }

  reference operator*() {
    return traits::dereference(value_);
  }

  pointer operator->() {
    return &value_;
  }

private:
  void read_next() {
    if (result_ == nullptr) {
      at_end_ = true;
      traits::clear(value_);
      return;
    }

    at_end_ = !traits::read_next(*result_, value_);
  }

private:
  value_type value_{};
  query_result<Type>* result_{nullptr};
  bool at_end_{true};
};

template<typename Type>
class query_result final {
public:
  using iterator = query_result_iterator<Type>;
  using storage_type = detail::query_result_storage<Type>;
  using traits = detail::query_result_traits<Type>;

  template<typename... Args>
  [[nodiscard]] static result<query_result, error> make_query_result(Args&&... args) {
    return traits::make_query_result(std::forward<Args>(args)...);
  }

  iterator begin() {
    return ++iterator(this);
  }

  iterator end() {
    return {};
  }

private:
  friend class query_result_iterator<Type>;
  friend struct detail::query_result_traits<Type>;
  friend struct detail::query_result_traits<record>;

  query_result(std::unique_ptr<query_result_impl>&& impl, storage_type storage)
: impl_(std::move(impl))
, storage_(std::move(storage)) {}


  auto create() {
    if constexpr (std::is_same_v<Type, record>) {
      record rec;
      int index{0};

      for (const auto& col : storage_.prototype) {
        rec.append({
          col.name(),
          col.type(),
          col.constraints(),
          col.size(),
          index++
        });
      }

      return rec;
    } else {
      return storage_.creator();
    }
  }

  void bind(const Type& obj) {
    impl_->bind(obj);
  }

  bool fetch(Type& obj) {
    return impl_->fetch(obj);
  }

private:
  std::unique_ptr<query_result_impl> impl_;
  storage_type storage_;
};

namespace detail {

template <typename Type>
void query_result_traits<Type>::clear(stored_type& value) {
  value.reset();
}

template <typename Type>
typename query_result_traits<Type>::dereference_type query_result_traits<Type>::dereference(stored_type& value) {
  return value;
}

template <typename Type>
bool query_result_traits<Type>::read_next(query_result<Type>& result, stored_type& value) {
  auto obj = result.create();
  result.bind(*obj);

  value.reset();

  if (!result.fetch(*obj)) {
    return false;
  }

  value = object_ptr<Type>(
    std::make_shared<object_proxy<Type>>(
      result.storage_.resolver,
      std::move(obj)
    )
  );

  return true;
}

template <typename Type>
result<typename query_result_traits<Type>::result_type, error>
query_result_traits<Type>::make_query_result(
  std::unique_ptr<query_result_impl>&& impl,
  std::shared_ptr<object_resolver<Type>> resolver,
  creator_func&& creator) {
  if (!impl) {
    return failure<error>(error{error_code::Failure, "query_result requires a valid query_result_impl"});
  }

  if (!creator) {
    return failure<error>(error{error_code::Failure, "query_result requires a valid creator function"});
  }

  return ok<result_type>(result_type{
    std::move(impl),
    storage_type{std::move(resolver), std::move(creator)}
  });
}

inline void query_result_traits<record>::clear(stored_type& value) {
  value.clear();
}

inline query_result_traits<record>::dereference_type query_result_traits<record>::dereference(stored_type& value) {
  return value;
}

inline bool query_result_traits<record>::read_next(query_result<record>& result, stored_type& value) {
  value = result.create();
  result.bind(value);

  if (!result.fetch(value)) {
    value.clear();
    return false;
  }

  return true;
}

inline result<query_result_traits<record>::result_type, error>
query_result_traits<record>::make_query_result(
  std::unique_ptr<query_result_impl>&& impl,
  std::vector<column> prototype) {
  if (!impl) {
    return failure<error>(error{error_code::Failure, "query_result<record> requires a valid query_result_impl"});
  }

  return ok<result_type>(result_type{
    std::move(impl),
    storage_type{std::move(prototype)}
  });
}
}

}

#endif //QUERY_QUERY_RESULT_HPP