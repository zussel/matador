#ifndef QUERY_RESULT_PK_RESOLVER_HPP
#define QUERY_RESULT_PK_RESOLVER_HPP

#include "matador/query/access.hpp"
#include "matador/query/column_options.hpp"
#include "matador/query/foreign_key_options.hpp"
#include "matador/query/primary_key_options.hpp"
#include "matador/query/identifier.hpp"
#include "matador/query/interface/query_result_reader.hpp"

#include <stack>

namespace matador::query {
class identifier_discover_service final {
public:
  explicit identifier_discover_service(query_result_reader &reader)
  : reader_(reader) {
  }

  template<class Type>
  identifier discover(Type &obj) {
    column_index_ = reader_.start_column_index();
    pk_.clear();
    access::process(*this, obj);

    return pk_;
  }

  template<typename BaseType>
  static void on_base(const BaseType &) {
  }

  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &/*value*/, const primary_key_options &attr) {
    if (!type_stack_.empty()) {
      return;
    }
    ValueType value;
    data_type_traits<ValueType>::read_value(reader_, id, column_index_++, value, attr.size());
    pk_ = value;
  }

  void on_revision(const char * /*id*/, uint64_t &/*rev*/) {
    ++column_index_;
  }

  template<class Type>
  void on_attribute(const char * /*id*/, Type &/*x*/, const column_options &/*attr*/) { ++column_index_; }

  void on_attribute(const char *id, const column_value &x, const column_options &attr);

  template<class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer &/*x*/, const foreign_key_options &attr) {
    on_foreign_key<typename Pointer::value_type>(attr.fetch());
  }

  template<class Pointer>
  void on_has_one(const char * /*id*/, Pointer &/*x*/, const char * /*join_column*/, const foreign_key_options &attr) {
    on_foreign_key<typename Pointer::value_type>(attr.fetch());
  }

  template<class ContainerType>
  static void on_has_many(const char * /*id*/, ContainerType &, const char *, const foreign_key_options &/*attr*/) {
  }

  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType & /*c*/, const char * /*join_column*/,
                                  const char * /*inverse_join_column*/, const foreign_key_options &/*attr*/) {
  }

  template<class ContainerType>
  static void on_has_many_to_many(const char * /*id*/, ContainerType & /*c*/, const foreign_key_options &/*attr*/) {
  }

private:
  class type_stack_guard {
  public:
    explicit type_stack_guard(std::stack<std::type_index> &stack, const std::type_index type)
    : stack_(stack) {
      stack_.push(type);
    }

    ~type_stack_guard() {
      stack_.pop();
    }

    type_stack_guard(const type_stack_guard &) = delete;
    type_stack_guard &operator=(const type_stack_guard &) = delete;

  private:
    std::stack<std::type_index> &stack_;
  };

  template<class Type>
  void on_foreign_key(const fetch_type fetch) {
    if (fetch == fetch_type::Lazy) {
      ++column_index_;
    } else {
      const Type obj{};
      type_stack_guard guard(type_stack_, typeid(Type));
      access::process(*this, obj);
    }
  }

private:
  size_t column_index_{};
  identifier pk_;
  query_result_reader &reader_;
  std::stack<std::type_index> type_stack_;
};
}

#endif //QUERY_RESULT_PK_RESOLVER_HPP
