#ifndef OOS_TYPED_COLUMN_SERIALIZER_HPP
#define OOS_TYPED_COLUMN_SERIALIZER_HPP

#include "matador/sql/sql.hpp"
#include "matador/sql/column.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/identifier.hpp"

#include <memory>
#include <functional>

namespace matador {
namespace detail {

/// @cond MATADOR_DEV

class typed_column_serializer;

class typed_column_identifier_serializer : public identifier_serializer
{
public:
  explicit typed_column_identifier_serializer(typed_column_serializer &serializer)
    : serializer_(serializer) {}

  void make_typed_column(const char *id, identifier &pk);

  void serialize(short &value, const field_attributes &attr) override { make_typed_column(value, attr); }
  void serialize(int &value, const field_attributes &attr) override { make_typed_column(value, attr); }
  void serialize(long &value, const field_attributes &attr) override { make_typed_column(value, attr); }
  void serialize(long long &value, const field_attributes &attr) override { make_typed_column(value, attr); }
  void serialize(unsigned short &value, const field_attributes &attr) override { make_typed_column(value, attr); }
  void serialize(unsigned int &value, const field_attributes &attr) override { make_typed_column(value, attr); }
  void serialize(unsigned long &value, const field_attributes &attr) override { make_typed_column(value, attr); }
  void serialize(unsigned long long &value, const field_attributes &attr) override { make_typed_column(value, attr); }
  void serialize(std::string &value, const field_attributes &attr) override { make_typed_column(value, attr); }
  void serialize(null_type_t &, const field_attributes &/*attr*/) override {}

private:
  template < typename ValueType >
  void make_typed_column(ValueType &value, const field_attributes &attr);

private:
  const char *id_{};
  typed_column_serializer &serializer_;
};

class typed_column_serializer
{
public:
  typed_column_serializer();
  ~typed_column_serializer() = default;

  template < class T >
  columns* execute(T &x)
  {
    cols_ = std::make_unique<columns>();
    matador::access::serialize(*this, x);
    return cols_.release();
  }

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  template < class V >
  void on_primary_key(const char *, V &x, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type* = 0);
  void on_primary_key(const char *id, std::string &, size_t size);
  void on_revision(const char *id, unsigned long long &/*rev*/);
  void on_attribute(const char *id, char &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, short &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, int &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, long &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, long long &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, unsigned char &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, unsigned short &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, unsigned int &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, unsigned long &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, unsigned long long &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, float &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, double &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, bool &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, char *x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, std::string &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, date &x, const field_attributes &/*attr*/ = null_attributes);
  void on_attribute(const char *id, time &x, const field_attributes &/*attr*/ = null_attributes);
  void on_belongs_to(const char *id, identifiable_holder &x, cascade_type);
  void on_has_one(const char *id, identifiable_holder &x, cascade_type);
  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

private:
  std::unique_ptr<columns> cols_;
  size_t index_ = 0;
//  typedef std::function<std::shared_ptr<column> (const char*, data_type, size_t)> t_create_column_func;
//  t_create_column_func create_column_func_;
//  typedef std::function<std::shared_ptr<column> (const char*, size_t, data_type, size_t)> t_create_varchar_column_func;
//  t_create_varchar_column_func create_varchar_column_func_;

  typed_column_identifier_serializer column_identifier_serializer_;
};

/// @endcond

//template < class T >
//std::shared_ptr<column> make_column(const char *id, data_type t, size_t index);
//
//template <>
//std::shared_ptr<column> make_column<typed_column>(const char *id, data_type t, size_t index);
//
//template <>
//std::shared_ptr<column> make_column<typed_identifier_column>(const char *id, data_type t, size_t index);
//
//template < class T >
//std::shared_ptr<column> make_varchar_column(const char *id, size_t s, data_type t, size_t index);
//
//template <>
//std::shared_ptr<column> make_varchar_column<typed_varchar_column>(const char *id, size_t s, data_type t, size_t index);
//
//template <>
//std::shared_ptr<column> make_varchar_column<identifier_varchar_column>(const char *id, size_t s, data_type t, size_t index);

template<typename V>
void typed_column_serializer::on_primary_key(const char *id, V &x, typename std::enable_if<std::is_integral<V>::value && !std::is_same<bool, V>::value>::type*)
{
//  create_column_func_ = make_column<typed_identifier_column>;
//  create_varchar_column_func_ = make_varchar_column<identifier_varchar_column>;
  on_attribute(id, x, { constraints::PRIMARY_KEY });
//  create_column_func_ = make_column<typed_column>;
//  create_varchar_column_func_ = make_varchar_column<typed_varchar_column>;
}

template<typename ValueType>
void typed_column_identifier_serializer::make_typed_column(ValueType &value, const field_attributes &attr)
{
  serializer_.on_attribute(id_, value, attr);
}

}
}

#endif //OOS_TYPED_COLUMN_SERIALIZER_HPP
