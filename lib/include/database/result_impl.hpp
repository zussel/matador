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

#include "object/object_producer.hpp"

#include <memory>

namespace oos {

class serializable;
class object_base_ptr;

namespace detail {

/// @cond OOS_DEV

class OOS_API result_impl
{
private:
  result_impl(const result_impl &) = delete;
  result_impl &operator=(const result_impl &) = delete;

public:
  typedef unsigned long size_type;

protected:
  result_impl(std::shared_ptr<object_base_producer> producer_);

public:
  virtual ~result_impl();

  void get(serializable *o);

  virtual const char *column(size_type c) const = 0;

  virtual bool fetch() = 0;

  /**
   * Fetch next line from database and
   * deserialized the given serializable.
   *
   * @param o Object to be deserialized
   * @return True if serializable was successfully deserialized
   */
  virtual bool fetch(serializable *) { return false; }

  virtual size_type affected_rows() const = 0;

  virtual size_type result_rows() const = 0;

  virtual size_type fields() const = 0;

  virtual int transform_index(int index) const = 0;

  std::shared_ptr<object_base_producer> producer() const;

protected:
  void read_foreign_object(const char *id, object_base_ptr &x);

protected:
  int result_index;

private:
  std::shared_ptr<object_base_producer> producer_;
};

/// @endcond

}

}

#endif /* RESULT_IMPL_HPP */
