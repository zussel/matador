#ifndef RESULT_IMPL_HPP
#define RESULT_IMPL_HPP

//#include "matador/sql/export.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/identifier.hpp"
#include "matador/utils/cascade_type.hpp"

#include <memory>

namespace matador {

class date;
class time;
class identifiable_holder;
class abstract_has_many;

namespace detail {

/// @cond MATADOR_DEV

class result_impl;

class result_identifier_reader : public identifier_serializer
{
public:
  explicit result_identifier_reader(result_impl &res)
  : result_impl_(res) {}

  void serialize(short &value) override { read_value(value); }
  void serialize(int &value) override { read_value(value); }
  void serialize(long &value) override { read_value(value); }
  void serialize(long long &value) override { read_value(value); }
  void serialize(unsigned short &value) override { read_value(value); }
  void serialize(unsigned int &value) override { read_value(value); }
  void serialize(unsigned long &value) override { read_value(value); }
  void serialize(unsigned long long &value) override { read_value(value); }
  void serialize(std::string &value) override { read_value(value); }
  void serialize(null_type_t &) override;

  template<class Type>
  void read_value(Type &value);

private:
  result_impl &result_impl_;
};

//class OOS_SQL_API result_impl
class result_impl
{
public:
  result_impl(const result_impl &) = delete;
  result_impl &operator=(const result_impl &) = delete;

public:
  typedef unsigned long size_type;

protected:
  result_impl();

  virtual bool needs_bind() { return false; };
  virtual bool finalize_bind() { return false; }
  virtual bool prepare_fetch() = 0;
  virtual bool finalize_fetch() = 0;

public:
  virtual ~result_impl() = default;

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  template<typename ValueType>
  void on_primary_key(const char *id, ValueType &value, long /*size*/ = -1)
  {
    read_value(id, column_index_++, value);
  }

  template<typename ValueType>
  void on_attribute(const char *id, ValueType &value, long /*size*/ = -1)
  {
    read_value(id, column_index_++, value);
  }

  void on_attribute(const char *id, char *, long size = -1);
  void on_attribute(const char *id, std::string&, long size = -1);
  void on_attribute(const char *id, matador::time&, long size = -1);
  void on_attribute(const char *id, matador::date&, long size = -1);

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
  virtual void read_value(const char *id, size_type index, char *value, long s) = 0;
  virtual void read_value(const char *id, size_type index, std::string &value) = 0;
  virtual void read_value(const char *id, size_type index, std::string &value, long s) = 0;

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
    if (needs_bind()) {
      serialize(*o);
      finalize_bind();
    }
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

protected:
  void read_foreign_object(const char *id, identifiable_holder &x);

  size_type column_index() const;

private:
  size_type column_index_ = 0;

  result_identifier_reader result_identifier_reader_;
};

/// @endcond

template<class Type>
void result_identifier_reader::read_value(Type &value) {
  result_impl_.on_attribute("", value);
}

}

}

#endif /* RESULT_IMPL_HPP */
