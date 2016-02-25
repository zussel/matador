#ifndef RESULT_IMPL_HPP
#define RESULT_IMPL_HPP

#ifdef _MSC_VER
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_API
#endif

#include "tools/access.hpp"
#include "tools/serializer.hpp"
#include "tools/cascade_type.hpp"

#include <memory>

namespace oos {

namespace detail {

/// @cond OOS_DEV

class OOS_API result_impl : public oos::serializer
{
private:
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
  virtual ~result_impl();

  template < class T >
  void get(T *o)
  {
    oos::access::serialize(*this, *o);
  }

  virtual void serialize(const char*, char&) = 0;
  virtual void serialize(const char*, short&) = 0;
  virtual void serialize(const char*, int&) = 0;
  virtual void serialize(const char*, long&) = 0;
  virtual void serialize(const char*, unsigned char&) = 0;
  virtual void serialize(const char*, unsigned short&) = 0;
  virtual void serialize(const char*, unsigned int&) = 0;
  virtual void serialize(const char*, unsigned long&) = 0;
  virtual void serialize(const char*, bool&) = 0;
  virtual void serialize(const char*, float&) = 0;
  virtual void serialize(const char*, double&) = 0;
  virtual void serialize(const char*, char *, size_t) = 0;
  virtual void serialize(const char*, std::string&) = 0;
  virtual void serialize(const char*, oos::varchar_base&) = 0;
  virtual void serialize(const char*, oos::time&) = 0;
  virtual void serialize(const char*, oos::date&) = 0;
  virtual void serialize(const char*, oos::basic_identifier &x) = 0;
  virtual void serialize(const char*, oos::identifiable_holder &x, cascade_type) = 0;

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
      oos::access::serialize(*this, *o);
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
    oos::access::serialize(*this, *o);
    return finalize_fetch();
  }

  template < class T >
  void serialize(T &x)
  {
    oos::access::serialize(*this, x);
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
