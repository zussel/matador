#ifndef RESULT_IMPL_HPP
#define RESULT_IMPL_HPP

#ifdef _MSC_VER
  #ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_SQL_API
#endif

#include "matador/utils/access.hpp"
#include "matador/utils/serializer.hpp"
#include "matador/utils/cascade_type.hpp"

#include <memory>

namespace matador {

namespace detail {

/// @cond MATADOR_DEV

class OOS_SQL_API result_impl : public matador::serializer
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
  ~result_impl() override = default;

  template < class T >
  void serialize(T &x)
  {
    matador::access::serialize(*this, x);
  }

  void serialize(const char*, char&) override = 0;
  void serialize(const char*, short&) override = 0;
  void serialize(const char*, int&) override = 0;
  void serialize(const char*, long&) override = 0;
  void serialize(const char*, unsigned char&) override = 0;
  void serialize(const char*, unsigned short&) override = 0;
  void serialize(const char*, unsigned int&) override = 0;
  void serialize(const char*, unsigned long&) override = 0;
  void serialize(const char*, bool&) override = 0;
  void serialize(const char*, float&) override = 0;
  void serialize(const char*, double&) override = 0;
  void serialize(const char*, char *, size_t) override = 0;
  void serialize(const char*, std::string&) override = 0;
  void serialize(const char*, std::string&, size_t) override = 0;
  void serialize(const char*, matador::time&) override = 0;
  void serialize(const char*, matador::date&) override = 0;
  void serialize(const char*, matador::basic_identifier &x) override = 0;
  void serialize(const char*, matador::identifiable_holder &x, cascade_type) override = 0;
  void serialize(const char *, abstract_has_many &, const char *, const char *, cascade_type) override {}
  void serialize(const char *, abstract_has_many &, cascade_type) override {}

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
};

/// @endcond

}

}

#endif /* RESULT_IMPL_HPP */
