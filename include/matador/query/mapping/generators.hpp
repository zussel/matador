#ifndef MATADOR_GENERATORS_HPP
#define MATADOR_GENERATORS_HPP

#include "matador/query/mapping/access.hpp"
#include "matador/query/builder/placeholder.hpp"
#include "matador/query/internal/column_value_pair.hpp"
#include "matador/query/internal/fk_value_extractor.hpp"

#include <vector>

namespace matador::query {
class primary_key_options;
class foreign_key_options;
class column_options;

class column_value_generator final {
public:
  explicit column_value_generator(bool generate_placeholder);
  template< class Type >
  std::vector<internal::column_value_pair> generate() {
      Type obj;
      return generate(obj);
  }

  template< class Type >
  std::vector<internal::column_value_pair> generate(const Type &obj) {
      result_.clear();
      access::process(*this, obj);

      return std::move(result_);
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template < class V >
  void on_primary_key(const char *id, V &x, const primary_key_options& /*attr*/) {
    push_back(id, x);
  }
  void on_revision(const char *id, uint64_t &rev);

  template<typename Type>
  void on_attribute(const char *id, Type &x, const column_options &/*attr*/) {
    push_back(id, x);
  }

  template<class Type, template < class ... > class Pointer>
  void on_belongs_to(const char *id, Pointer<Type> &x, const foreign_key_options &/*attr*/) {
    push_back(id, fk_value_extractor_.extract(*x));
  }
  template<class Type, template < class ... > class Pointer>
  void on_has_one(const char *id, Pointer<Type> &x, const char * /*join_column*/, const foreign_key_options &/*attr*/) {
    push_back(id, fk_value_extractor_.extract(*x));
  }
  template<class ContainerType>
  static void on_has_many(const char * /*id*/, ContainerType &, const char *, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char *, ContainerType &, const char *, const char *, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char *, ContainerType &, const foreign_key_options &/*attr*/) {}

  template<typename Type>
  void push_back(const char* id, const Type& value) {
    if (generate_placeholder_) {
      result_.emplace_back(id, std::make_unique<placeholder_expression>());
    } else {
      result_.emplace_back(id, std::make_unique<value_expression>(value));
    }
  }

private:
  detail::fk_value_extractor fk_value_extractor_{};
  std::vector<internal::column_value_pair> result_;
  bool generate_placeholder_{false};
};

class placeholder_generator final {
public:
  template< class Type >
  std::vector<placeholder> generate() {
    Type obj;
    return generate(obj);
  }

  template< class Type >
  std::vector<placeholder> generate(const Type &obj) {
    result_.clear();
    access::process(*this, obj);

    return result_;
  }

  template<typename BaseType>
  static void on_base(const BaseType&) {}
  template < class V >
  void on_primary_key(const char * /*id*/, V &/*x*/, const primary_key_options& /*attr*/) {
    result_.emplace_back(_);
  }
  void on_revision(const char *id, uint64_t &/*rev*/);

  template<typename Type>
  void on_attribute(const char * /*id*/, Type &/*x*/, const column_options &/*attr*/) {
    result_.emplace_back(_);
  }

  template<class Type, template < class ... > class Pointer>
  void on_belongs_to(const char * /*id*/, Pointer<Type> &/*x*/, const foreign_key_options &/*attr*/) {
    result_.emplace_back(_);
  }
  template<class Type, template < class ... > class Pointer>
  void on_has_one(const char * /*id*/, Pointer<Type> &/*x*/, const char * /*join_column*/, const foreign_key_options &/*attr*/) {
    result_.emplace_back(_);
  }
  template<class ContainerType>
  static void on_has_many(const char * /*id*/, ContainerType &, const char *, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char *, ContainerType &, const char *, const char *, const foreign_key_options &/*attr*/) {}
  template<class ContainerType>
  static void on_has_many_to_many(const char *, ContainerType &, const foreign_key_options &/*attr*/) {}

private:
  std::vector<placeholder> result_;
};

template<typename Type>
std::vector<internal::column_value_pair> column_value_pairs(const Type &obj, const bool generate_placeholder = false) {
  column_value_generator generator(generate_placeholder);
  return generator.generate(obj);
}

template<typename Type>
std::vector<internal::column_value_pair> column_value_pairs() {
  Type obj;
  return column_value_pairs(obj, true);
}

template<typename Type>
std::vector<placeholder> placeholders() {
  placeholder_generator generator;
  return generator.generate<Type>();
}

template<typename Type>
std::vector<placeholder> placeholders(const Type &obj) {
  placeholder_generator generator;
  return generator.generate(obj);
}

std::vector<placeholder> placeholders(size_t num);

}
#endif // MATADOR_GENERATORS_HPP
