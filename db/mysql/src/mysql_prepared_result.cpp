#include "mysql_prepared_result.hpp"
#include "mysql_exception.hpp"

#include "tools/date.hpp"
#include "tools/time.hpp"
#include "tools/varchar.hpp"
#include "tools/basic_identifier.hpp"
#include "tools/identifiable_holder.hpp"

#include <cstring>

namespace oos {

namespace mysql {

mysql_prepared_result::mysql_prepared_result(MYSQL_STMT *s, int rs)
  : affected_rows_((size_type)mysql_stmt_affected_rows(s))
  , rows((size_type)mysql_stmt_num_rows(s))
  , fields_(mysql_stmt_field_count(s))
  , stmt(s)
  , result_size(rs)
  , bind_(new MYSQL_BIND[rs])
  , info_(new mysql_result_info[rs])
{
    memset(bind_, 0, rs * sizeof(MYSQL_BIND));
    memset(info_, 0, rs * sizeof(mysql_result_info));
}

mysql_prepared_result::~mysql_prepared_result()
{
  delete [] bind_;
  for (int i = 0; i < result_size; ++i) {
    if (info_[i].buffer != 0) {
      delete [] info_[i].buffer;
    }
  }
  delete [] info_;
}

const char* mysql_prepared_result::column(size_type ) const
{
  return "not implemented";
}

bool mysql_prepared_result::fetch()
{
  // get next row
  int ret = mysql_stmt_fetch(stmt);
  if (ret == MYSQL_DATA_TRUNCATED) {
    // Todo: handle truncated data
  }
  return rows-- > 0;
}

bool mysql_prepared_result::prepare_fetch()
{
  // reset result column index
  result_index_ = 0;
  // fetch data
  int ret = mysql_stmt_fetch(stmt);
  if (ret == MYSQL_NO_DATA) {
    return false;
  } else if (ret == 1) {
    throw_stmt_error(ret, stmt, "mysql", "");
  }
  prepare_binding_ = false;
  return true;
}

bool mysql_prepared_result::finalize_fetch()
{
  return true;
}

mysql_prepared_result::size_type mysql_prepared_result::affected_rows() const
{
  size_t ar = mysql_stmt_affected_rows(stmt);
  return ar;
  //return affected_rows_;
}

mysql_prepared_result::size_type mysql_prepared_result::result_rows() const
{
  return rows;
}

mysql_prepared_result::size_type mysql_prepared_result::fields() const
{
  return fields_;
}

int mysql_prepared_result::transform_index(int index) const
{
  return index;
}

void mysql_prepared_result::serialize(const char */*id*/, char &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_TINY, x);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, short & x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_SHORT, x);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, int &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_LONG, x);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, long &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_LONGLONG, x);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, unsigned char &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_TINY, x);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, unsigned short &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_SHORT, x);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, unsigned int &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_LONG, x);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, unsigned long &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_LONGLONG, x);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, bool &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_TINY, x);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, float &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_FLOAT, x);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, double &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_DOUBLE, x);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, char *x, size_t s)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_VAR_STRING, x, s);
  } else {
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, oos::date &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_DATE, x);
  } else {
    if (info_[result_index_].length > 0) {
      MYSQL_TIME *mtt = (MYSQL_TIME*)info_[result_index_].buffer;
      x.set(mtt->day, mtt->month, mtt->year);
    }
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, oos::time &x)
{
  if (prepare_binding_) {
#if MYSQL_VERSION_ID < 50604
    prepare_bind_column(column_index_++, MYSQL_TYPE_VAR_STRING, x);
#else
    prepare_bind_column(column_index_++, MYSQL_TYPE_TIMESTAMP, x);
#endif
  } else {
    if (info_[result_index_].length > 0) {
#if MYSQL_VERSION_ID < 50604
      // before mysql version 5.6.4 datetime
    // doesn't support fractional seconds
    // so we use a datetime string here
      std::string val;
      serialize(id, val);
      x = oos::time::parse(val, "%F %T");
#else
      MYSQL_TIME *mtt = (MYSQL_TIME*)info_[result_index_].buffer;
      x.set(mtt->year, mtt->month, mtt->day, mtt->hour, mtt->minute, mtt->second, mtt->second_part / 1000);
#endif
    }
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, std::string &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_STRING, x);
  } else {
    if (info_[result_index_].length > 0) {
      bind_[result_index_].buffer = new char[info_[result_index_].length];
      bind_[result_index_].buffer_length = info_[result_index_].length;
      if (mysql_stmt_fetch_column(stmt, &bind_[result_index_], result_index_, 0) != 0) {
        // an error occured
      } else {
        char *data = (char*)bind_[result_index_].buffer;
        unsigned long len = bind_[result_index_].buffer_length;
        x.assign(data, len);
      }
      delete [] (char*)bind_[result_index_].buffer;
      bind_[result_index_].buffer = 0;
    }
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char */*id*/, varchar_base &x)
{
  if (prepare_binding_) {
    prepare_bind_column(column_index_++, MYSQL_TYPE_VAR_STRING, x);
  } else {
    char *data = (char*)bind_[result_index_].buffer;
//  unsigned long len = bind_[result_index].buffer_length;
    unsigned long len = info_[result_index_].length;
    x.assign(data, len);
    ++result_index_;
  }
}

void mysql_prepared_result::serialize(const char *id, oos::basic_identifier &x)
{
  x.serialize(id, *this);
}

void mysql_prepared_result::serialize(const char *id, identifiable_holder &x, cascade_type)
{
  if (prepare_binding_) {
    std::shared_ptr<basic_identifier> pk(x.create_identifier());
    pk->serialize(id, *this);
    foreign_keys_.insert(std::make_pair(id, pk));
  } else {
    t_foreign_key_map::iterator i = foreign_keys_.find(id);
    if (i != foreign_keys_.end()) {
      if (i->second->is_valid()) {
        x.reset(i->second);
      }
      foreign_keys_.erase(i);
    }
  }
}

bool mysql_prepared_result::needs_bind()
{
  prepare_binding_ = true;
  column_index_ = 0;
  return prepare_binding_;
}

bool mysql_prepared_result::finalize_bind()
{
  // Todo: validate result
  return mysql_stmt_bind_result(stmt, bind_);
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, oos::date &)
{
  if (info_[index].buffer == 0) {
    size_t s = sizeof(MYSQL_TIME);
    info_[index].buffer = new char[s];
    memset(info_[index].buffer, 0, s);
    info_[index].buffer_length = s;
  }
  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, oos::time &)
{
  if (info_[index].buffer == 0) {
    size_t s = sizeof(MYSQL_TIME);
    info_[index].buffer = new char[s];
    memset(info_[index].buffer, 0, s);
    info_[index].buffer_length = s;
  }
  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, std::string & /*value*/)
{
  bind_[index].buffer_type = type;
  bind_[index].buffer = nullptr;
  bind_[index].buffer_length = 0;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, char *x, size_t s)
{
  bind_[index].buffer_type = type;
  bind_[index].buffer= x;
  bind_[index].buffer_length = (unsigned long) s;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, varchar_base &x)
{
  if (info_[index].buffer == nullptr) {
    info_[index].buffer = new char[x.capacity()];
    memset(info_[index].buffer, 0, x.capacity());
    info_[index].buffer_length = x.capacity();
  }
  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

}

}
