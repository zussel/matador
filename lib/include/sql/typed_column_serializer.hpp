//
// Created by sascha on 2/25/16.
//

#ifndef OOS_TYPED_COLUMN_SERIALIZER_HPP
#define OOS_TYPED_COLUMN_SERIALIZER_HPP

#include "sql/sql.hpp"
#include "sql/column.hpp"

#include "tools/access.hpp"
#include "tools/cascade_type.hpp"
#include "tools/serializer.hpp"

namespace oos {
namespace detail {

class typed_column_serializer : public serializer
{
public:
  explicit typed_column_serializer(sql &d);
  virtual ~typed_column_serializer() {}

  template < class T >
  columns* execute(T &x)
  {
    cols_.reset(new columns);
    oos::access::serialize(static_cast<serializer&>(*this), x);
    return cols_.release();
  }

//  template < class T >
//  void serialize(T &x)
//  {
//    oos::access::serialize(*this, x);
//  }

  void serialize(const char *id, char &x);
  void serialize(const char *id, short &x);
  void serialize(const char *id, int &x);
  void serialize(const char *id, long &x);
  void serialize(const char *id, unsigned char &x);
  void serialize(const char *id, unsigned short &x);
  void serialize(const char *id, unsigned int &x);
  void serialize(const char *id, unsigned long &x);
  void serialize(const char *id, float &x);
  void serialize(const char *id, double &x);
  void serialize(const char *id, bool &x);
  void serialize(const char *id, char *x, size_t s);
  void serialize(const char *id, varchar_base &x);
  void serialize(const char *id, std::string &x);
  void serialize(const char *id, date &x);
  void serialize(const char *id, time &x);
  void serialize(const char *id, identifiable_holder &x, cascade_type);
  void serialize(const char *id, basic_identifier &x);

private:
private:
  std::unique_ptr<detail::columns> cols_;
  size_t index_ = 0;
  sql &sql_;
  typedef std::function<std::shared_ptr<column> (const char*, data_type_t, size_t)> t_create_column_func;
  t_create_column_func create_column_func_;
  typedef std::function<std::shared_ptr<column> (const char*, size_t, data_type_t, size_t)> t_create_varchar_column_func;
  t_create_varchar_column_func create_varchar_column_func_;
};

}

}

#endif //OOS_TYPED_COLUMN_SERIALIZER_HPP
