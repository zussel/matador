#ifndef OOS_COLUMN_SERIALIZER_HPP
#define OOS_COLUMN_SERIALIZER_HPP

#include "matador/sql/sql.hpp"
#include "matador/sql/column.hpp"
#include "matador/sql/columns.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/cascade_type.hpp"

namespace matador {


namespace detail {

template < class ColumnSerializer >
class query_column_serializer
{
public:
  explicit query_column_serializer(columns::t_brackets brackets)
  : brackets_(brackets)
  {}

  template<class T>
  std::unique_ptr<columns> execute(T &x) {
    cols_ = std::make_unique<columns>(brackets_);
    
    matador::access::process(column_serializer_, x);
    return std::move(cols_);
  }

private:
  std::unique_ptr<columns> cols_;
  columns::t_brackets brackets_;
  ColumnSerializer column_serializer_;
};

/// @cond MATADOR_DEV

class column_serializer
{
public:
  template<class T>
  std::unique_ptr<columns> execute(T &x, columns::t_brackets brackets) {
    cols_ = std::make_unique<columns>(brackets);
    matador::access::process(*this, x);
    return std::move(cols_);
  }

  template < class T >
  void serialize(T &x)
  {
    matador::access::process(*this, x);
  }

  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &, typename std::enable_if<std::is_integral<ValueType>::value && !std::is_same<bool, ValueType>::value>::type* = 0)
  {
    cols_->emplace_back(id);
  }
  void on_primary_key(const char *id, std::string &, size_t size) ;
  void on_revision(const char *id, unsigned long long &rev);

  template<typename Type>
  void on_attribute(const char *id, Type &/*x*/, const field_attributes &/*attr*/ = null_attributes)
  {
    cols_->emplace_back(id);
  }
  void on_attribute(const char *id, char *x, const field_attributes &/*attr*/ = null_attributes);
  void on_belongs_to(const char *id, identifiable_holder &x, cascade_type);

  void on_has_one(const char *id, identifiable_holder &x, cascade_type);
  void on_has_many(const char *, abstract_container &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_container &, cascade_type) {}

private:
  std::unique_ptr<columns> cols_;
};

/// @endcond

}
}

#endif //OOS_COLUMN_SERIALIZER_HPP
