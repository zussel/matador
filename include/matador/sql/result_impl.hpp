#ifndef RESULT_IMPL_HPP
#define RESULT_IMPL_HPP

#include "matador/sql/export.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/serializer.hpp"
#include "matador/utils/cascade_type.hpp"

#include <memory>

namespace matador {

namespace detail {

/// @cond MATADOR_DEV

class OOS_SQL_API result_impl
{
public:
  result_impl(const result_impl &) = delete;
  result_impl &operator=(const result_impl &) = delete;

public:
  typedef unsigned long size_type;

protected:
  result_impl() = default;

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
  void on_primary_key(const char*, ValueType &value, long size = -1)
  {
    read_value(result_index_++, result_row_, value);
  }

  template<typename ValueType>
  void on_attribute(const char*, ValueType &value, long size = -1)
  {
    read_value(result_index_++, result_row_, value);
  }

  void on_attribute(const char*, char *, long size = -1);
  void on_attribute(const char*, std::string&, long size = -1);
  void on_attribute(const char*, matador::time&, long size = -1);
  void on_attribute(const char*, matador::date&, long size = -1);

  void on_belongs_to(const char *id, matador::identifiable_holder &x, cascade_type);
  void on_has_one(const char *id, matador::identifiable_holder &x, cascade_type);

  void on_has_many(const char *, abstract_has_many &, const char *, const char *, cascade_type) {}
  void on_has_many(const char *, abstract_has_many &, cascade_type) {}

  virtual void read_value(int index, int row, char &value) = 0;
  virtual void read_value(int index, int row, short &value) = 0;
  virtual void read_value(int index, int row, int &value) = 0;
  virtual void read_value(int index, int row, long &value) = 0;
  virtual void read_value(int index, int row, long long &value) = 0;
  virtual void read_value(int index, int row, unsigned char &value) = 0;
  virtual void read_value(int index, int row, unsigned short &value) = 0;
  virtual void read_value(int index, int row, unsigned int &value) = 0;
  virtual void read_value(int index, int row, unsigned long &value) = 0;
  virtual void read_value(int index, int row, unsigned long long &value) = 0;
  virtual void read_value(int index, int row, bool &value) = 0;
  virtual void read_value(int index, int row, float &value) = 0;
  virtual void read_value(int index, int row, double &value) = 0;
  virtual void read_value(int index, int row, matador::time &value) = 0;
  virtual void read_value(int index, int row, matador::date &value) = 0;
  virtual void read_value(int index, int row, char *value, size_t s) = 0;
  virtual void read_value(int index, int row, std::string &value) = 0;
  virtual void read_value(int index, int row, std::string &value, size_t s) = 0;

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
    result_index_ = transform_index(0);
    serialize(*o);
    return finalize_fetch();
  }

  virtual size_type affected_rows() const = 0;

  virtual size_type result_rows() const = 0;

  virtual size_type fields() const = 0;

  virtual int transform_index(int index) const = 0;

protected:
  void read_foreign_object(const char *id, identifiable_holder &x);

protected:
  int result_index_ = 0;
  int result_row_ = 0;
};

/// @endcond

}

}

#endif /* RESULT_IMPL_HPP */
