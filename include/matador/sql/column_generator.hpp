#ifndef QUERY_COLUMN_GENERATOR_HPP
#define QUERY_COLUMN_GENERATOR_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/foreign_attributes.hpp"

#include "matador/sql/column.hpp"
#include "matador/sql/schema.hpp"

#include <string>
#include <vector>
#include <stack>

namespace matador::sql {

class column_generator
{
private:
  column_generator(std::vector<column> &column_infos,
                   const sql::schema &ts,
                   const std::string &table_name,
                   bool force_lazy);

public:
  ~column_generator() = default;

  template < class Type >
  static std::vector<column> generate(const sql::schema &ts, bool force_lazy = false)
  {
    const auto info = ts.info<Type>();
    if (!info) {
      return {};
    }
    std::vector<column> columns;
    column_generator gen(columns, ts, info.value().name, force_lazy);
    Type obj;
    matador::access::process(gen, obj);
    return columns;
  }

  template < class V >
  void on_primary_key(const char *id, V &, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0)
  {
    push(id);
  }
  void on_primary_key(const char *id, std::string &, size_t);
  void on_revision(const char *id, unsigned long long &/*rev*/);

  template<typename Type>
  void on_attribute(const char *id, Type &, const utils::field_attributes &/*attr*/ = utils::null_attributes)
  {
    push(id);
  }

  template<class Pointer>
  void on_belongs_to(const char *id, Pointer &, const utils::foreign_attributes &attr)
  {
    if (attr.fetch() == utils::fetch_type::LAZY || force_lazy_) {
      push(id);
    } else {
      const auto info = table_schema_.info<typename Pointer::value_type>();
      if (!info) {
        return;
      }
      table_name_stack_.push(info.value().name);
      typename Pointer::value_type obj;
      matador::access::process(*this, obj);
      table_name_stack_.pop();
    }
  }
  template<class Pointer>
  void on_has_one(const char *id, Pointer &, const utils::foreign_attributes &attr)
  {
    if (attr.fetch() == utils::fetch_type::LAZY || force_lazy_) {
      push(id);
    } else {
      const auto info = table_schema_.info<typename Pointer::value_type>();
      if (!info) {
        return;
      }
      table_name_stack_.push(info.value().name);
      typename Pointer::value_type obj;
      matador::access::process(*this, obj);
      table_name_stack_.pop();
    }
  }
  template<class ContainerType>
  void on_has_many(const char * /*id*/, ContainerType &, const char *, const utils::foreign_attributes &attr)
  {
    if (attr.fetch() == utils::fetch_type::LAZY || force_lazy_) {
      return;
    }
    const auto info = table_schema_.info<typename ContainerType::value_type::value_type>();
    if (!info) {
      return;
    }

    table_name_stack_.push(info.value().name);
    typename ContainerType::value_type::value_type obj;
    matador::access::process(*this, obj);
    table_name_stack_.pop();
  }

  template<class ContainerType>
  void on_has_many_to_many(const char * /*id*/, ContainerType &/*c*/, const char * /*join_column*/, const char * /*inverse_join_column*/, const utils::foreign_attributes &/*attr*/)
  {
  }

  template<class ContainerType>
  void on_has_many_to_many(const char * /*id*/, ContainerType &/*c*/, const utils::foreign_attributes &/*attr*/)
  {
  }

private:
  void push(const std::string &column_name);

private:
  std::stack<std::string> table_name_stack_;
  std::vector<column> &column_infos_;
  const sql::schema &table_schema_;
  int column_index{0};
  bool force_lazy_{false};
};

}

#endif //QUERY_COLUMN_GENERATOR_HPP
