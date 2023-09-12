#ifndef RESULT_IMPL_HPP
#define RESULT_IMPL_HPP

#include "matador/utils/access.hpp"
#include "matador/utils/field_attributes.hpp"
#include "matador/utils/identifier.hpp"
#include "matador/utils/cascade_type.hpp"
#include "matador/utils/serializer.hpp"

#include "matador/sql/value_processor.hpp"

#include <memory>

namespace matador {

class date;
class time;
class identifiable_holder;
class abstract_has_many;
class row;

namespace detail {

/// @cond MATADOR_DEV

class result_impl;

class result_row_serializer : public serializer
{
public:
  explicit result_row_serializer(result_impl &impl);

  void process(const char *id, value &val, const field_attributes &attr);

  void on_attribute(const char *id, char &x, const field_attributes &attr) override;
  void on_attribute(const char *id, short &x, const field_attributes &attr) override;
  void on_attribute(const char *id, int &x, const field_attributes &attr) override;
  void on_attribute(const char *id, long &x, const field_attributes &attr) override;
  void on_attribute(const char *id, long long &x, const field_attributes &attr) override;
  void on_attribute(const char *id, unsigned char &x, const field_attributes &attr) override;
  void on_attribute(const char *id, unsigned short &x, const field_attributes &attr) override;
  void on_attribute(const char *id, unsigned int &x, const field_attributes &attr) override;
  void on_attribute(const char *id, unsigned long &x, const field_attributes &attr) override;
  void on_attribute(const char *id, unsigned long long &x, const field_attributes &attr) override;
  void on_attribute(const char *id, bool &x, const field_attributes &attr) override;
  void on_attribute(const char *id, float &x, const field_attributes &attr) override;
  void on_attribute(const char *id, double &x, const field_attributes &attr) override;
  void on_attribute(const char *id, char *, const field_attributes &attr) override;
  void on_attribute(const char *id, std::string&, const field_attributes &attr) override;
  void on_attribute(const char *id, matador::time&, const field_attributes &attr) override;
  void on_attribute(const char *id, matador::date&, const field_attributes &attr) override;

  void on_belongs_to(const char *id, matador::identifiable_holder &x, cascade_type) override;
  void on_has_one(const char *id, matador::identifiable_holder &x, cascade_type) override;

  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) override {}

private:
  result_impl &impl_;
  detail::value_processor value_processor_;
};

class result_identifier_reader : public identifier_serializer
{
public:
  explicit result_identifier_reader(result_impl &res)
  : result_impl_(res) {}

  void serialize(short &value, const field_attributes &attr) override { read_value(value, attr); }
  void serialize(int &value, const field_attributes &attr) override { read_value(value, attr); }
  void serialize(long &value, const field_attributes &attr) override { read_value(value, attr); }
  void serialize(long long &value, const field_attributes &attr) override { read_value(value, attr); }
  void serialize(unsigned short &value, const field_attributes &attr) override { read_value(value, attr); }
  void serialize(unsigned int &value, const field_attributes &attr) override { read_value(value, attr); }
  void serialize(unsigned long &value, const field_attributes &attr) override { read_value(value, attr); }
  void serialize(unsigned long long &value, const field_attributes &attr) override { read_value(value, attr); }
  void serialize(std::string &value, const field_attributes &attr) override { read_value(value, attr); }
  void serialize(null_type_t &, const field_attributes &/*attr*/) override;

  template<class Type>
  void read_value(Type &value, const field_attributes &attr);

private:
  result_impl &result_impl_;
};

class result_impl
{
public:
  result_impl(const result_impl &) = delete;
  result_impl &operator=(const result_impl &) = delete;

  virtual ~result_impl() = default;

public:
  typedef unsigned long size_type;

protected:
  result_impl();

  virtual bool needs_bind() { return false; };
  virtual bool finalize_bind() { return false; }
  virtual bool prepare_fetch() = 0;
  virtual bool finalize_fetch() = 0;

public:
  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  void serialize(row &r);

  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &value, typename std::enable_if<std::is_integral<ValueType>::value && !std::is_same<bool, ValueType>::value>::type* = 0)
  {
    read_value(id, column_index_++, value);
  }
  void on_primary_key(const char *id, std::string &v, size_t size);
  void on_revision(const char *id, unsigned long long &rev);

  template < class Type >
  void on_attribute(const char *id, Type &x, const field_attributes &/*attr*/ = null_attributes)
  {
    read_value(id, column_index_++, x);
  }
  void on_attribute(const char *id, char *, const field_attributes &attr = null_attributes);
  void on_attribute(const char *id, std::string&, const field_attributes &attr = null_attributes);

  void on_belongs_to(const char *id, matador::identifiable_holder &x, cascade_type);
  void on_has_one(const char *id, matador::identifiable_holder &x, cascade_type);

  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

  virtual void read_value(const char *id, size_type index, char &value) = 0;
  virtual void read_value(const char *id, size_type index, short &value) = 0;
  virtual void read_value(const char *id, size_type index, int &value) = 0;
  virtual void read_value(const char *id, size_type index, long &value) = 0;
  virtual void read_value(const char *id, size_type index, long long &value) = 0;
  virtual void read_value(const char *id, size_type index, unsigned char &value) = 0;
  virtual void read_value(const char *id, size_type index, unsigned short &value) = 0;
  virtual void read_value(const char *id, size_type index, unsigned int &value) = 0;
  virtual void read_value(const char *id, size_type index, unsigned long &value) = 0;
  virtual void read_value(const char *id, size_type index, unsigned long long &value) = 0;
  virtual void read_value(const char *id, size_type index, bool &value) = 0;
  virtual void read_value(const char *id, size_type index, float &value) = 0;
  virtual void read_value(const char *id, size_type index, double &value) = 0;
  virtual void read_value(const char *id, size_type index, matador::time &value) = 0;
  virtual void read_value(const char *id, size_type index, matador::date &value) = 0;
  virtual void read_value(const char *id, size_type index, char *value, size_t s) = 0;
  virtual void read_value(const char *id, size_type index, std::string &value) = 0;
  virtual void read_value(const char *id, size_type index, std::string &value, size_t s) = 0;

  virtual const char *column(size_type c) const = 0;

  virtual bool fetch() = 0;

  /**
   * Fetch next line from sql and
   * deserialized the given serializable.
   *
   * @param o Object to be deserialized
   * @return True if serializable was successfully deserialized
   */
  template < class T >
  void bind(T *o)
  {
    if (!needs_bind()) {
      return;
    }
    column_index_ = reset_column_index();
    serialize(*o);
    finalize_bind();
  }

  template < class T >
  bool fetch(T *o)
  {
    if (!prepare_fetch()) {
      return false;
    }
    column_index_ = reset_column_index();
    serialize(*o);
    return finalize_fetch();
  }

  virtual size_type affected_rows() const = 0;
  virtual size_type result_rows() const = 0;
  virtual size_type fields() const = 0;

  virtual size_type reset_column_index() const = 0;

  virtual void close() = 0;

protected:
  friend class result_row_serializer;

protected:
  virtual void read_foreign_object(const char *id, identifiable_holder &x);

  size_type column_index() const;

  result_identifier_reader result_identifier_reader_;

  size_type column_index_ = 0;

private:
  result_row_serializer result_row_serializer_;
};

/// @endcond

template<class Type>
void result_identifier_reader::read_value(Type &value, const field_attributes &attr) {
  result_impl_.on_attribute("", value, attr);
}

}

}

#endif /* RESULT_IMPL_HPP */
