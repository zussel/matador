#include "matador/db/mysql/mysql_prepared_result.hpp"
#include "matador/db/mysql/mysql_statement.hpp"
#include "matador/db/mysql/mysql_exception.hpp"
#include "matador/db/mysql/mysql_constants.hpp"

#include "matador/utils/date.hpp"
#include "matador/utils/time.hpp"
#include "matador/utils/identifiable_holder.hpp"

#include <cstring>

namespace matador {

namespace mysql {

mysql_prepared_result::mysql_prepared_result(mysql_statement *owner, MYSQL_STMT *s, std::vector<MYSQL_BIND> &bind, std::vector<mysql_result_info> &info)
  : affected_rows_((size_type)mysql_stmt_affected_rows(s))
  , rows((size_type)mysql_stmt_num_rows(s))
  , fields_(mysql_stmt_field_count(s))
  , owner_(owner)
  , stmt(s)
  , bind_(bind)
  , info_(info)
{
}

mysql_prepared_result::~mysql_prepared_result()
{
  free();
}

void mysql_prepared_result::free()
{
  if (stmt != nullptr) {
    mysql_stmt_free_result(stmt);
    stmt = nullptr;
  }

  if (owner_ != nullptr) {
    owner_->unlink_result(this);
    owner_ = nullptr;
  }
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
  // fetch data
  int ret = mysql_stmt_fetch(stmt);
  if (ret == MYSQL_NO_DATA) {
    return false;
  } else if (ret == 1) {
    throw_stmt_error(stmt, "mysql", "");
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
  std::size_t ar = mysql_stmt_affected_rows(stmt);
  return (unsigned long)ar;
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

detail::result_impl::size_type mysql_prepared_result::reset_column_index() const
{
  return 0;
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, char &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_TINY, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, short &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_SHORT, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, int &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONG, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, long &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONGLONG, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, long long &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONGLONG, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, unsigned char &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_VAR_STRING, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, unsigned short &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_SHORT, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, unsigned int &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONG, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, unsigned long &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONGLONG, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, unsigned long long &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_LONGLONG, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, bool &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_TINY, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, float &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_FLOAT, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, double &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_DOUBLE, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, char *value, size_t size) {
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_VAR_STRING, value, size);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, matador::date &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_DATE, value);
  } else {
    if (info_[index].length > 0) {
      auto *mtt = (MYSQL_TIME*)info_[index].buffer;
      value.set(mtt->day, mtt->month, mtt->year);
    }
    bind_[index].length = nullptr;
//    increase_column_index();
  }
}

void mysql_prepared_result::read_value(const char *id, size_type index, matador::time &value)
{
  if (mysql::version < 50604) {
    if (prepare_binding_) {
      prepare_bind_column(index, MYSQL_TYPE_VAR_STRING, value);
    } else {
      if (info_[index].length > 0) {
        // before mysql version 5.6.4 datetime
        // doesn't support fractional seconds,
        // so we use a datetime string here
        std::string val;
        read_value(id, index, val);
        value = matador::time::parse(val, "%FT%T");
//        increase_column_index();
      }
    }
  } else {
    if (prepare_binding_) {
      prepare_bind_column(index, MYSQL_TYPE_TIMESTAMP, value);
    } else {
      if (info_[index].length > 0) {
        auto *mtt = (MYSQL_TIME*)info_[index].buffer;
        value.set(mtt->year, mtt->month, mtt->day, mtt->hour, mtt->minute, mtt->second, mtt->second_part / 1000);
      }
      bind_[index].length = nullptr;
    }
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, std::string &value)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_STRING, value);
  } else {
    on_truncated_data(index, value);
  }
}

void mysql_prepared_result::read_value(const char */*id*/, size_type index, std::string &value, size_t size)
{
  if (prepare_binding_) {
    prepare_bind_column(index, MYSQL_TYPE_VAR_STRING, value, size);
  } else {
    if (*bind_[index].error) {
      // assume truncated data
      on_truncated_data(index, value);
    } else {
      auto *data = (char *) bind_[index].buffer;
      unsigned long len = info_[index].length;
      value.assign(data, len);
    }
  }
}

void mysql_prepared_result::read_foreign_object(const char *id, identifiable_holder &x)
{
  if (prepare_binding_) {
    auto pk = x.create_identifier();
    pk.serialize(result_identifier_reader_);
    foreign_keys_.insert(std::make_pair(id, std::move(pk)));
  } else {
    auto i = foreign_keys_.find(id);
    if (i != foreign_keys_.end()) {
      if (i->second.is_valid()) {
//        std::cout << "created identifier " << *i->second << " (field: " << i->first << ", " << i->second.get() << ")\n";
        x.reset(i->second);
      }
      foreign_keys_.erase(i);
    }
    ++column_index_;
  }
}

void mysql_prepared_result::on_truncated_data(int index, std::string &x) {
  if (info_[index].length == 0) {
    return;
  }
  bind_[index].buffer = new char[info_[index].length];
  bind_[index].buffer_length = info_[index].length;
  if (mysql_stmt_fetch_column(stmt, &bind_[index], index, 0) == 0) {
    auto *data = (char*)bind_[index].buffer;
    unsigned long len = bind_[index].buffer_length;
    x.assign(data, len);
  } else {
      // Todo: handle statement fetch column error
  }
  delete [] (char*)bind_[index].buffer;
  bind_[index].buffer = nullptr;
  bind_[index].length = nullptr;
}

bool mysql_prepared_result::needs_bind()
{
  prepare_binding_ = true;
  return prepare_binding_;
}

bool mysql_prepared_result::finalize_bind()
{
  // Todo: validate result
  return mysql_stmt_bind_result(stmt, &bind_.front()) > 0;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, matador::date &)
{
  if (info_[index].buffer == nullptr) {
    size_t s = sizeof(MYSQL_TIME);
    info_[index].buffer = new char[s];
    memset(info_[index].buffer, 0, s);
    info_[index].buffer_length = (unsigned long)s;
    info_[index].is_allocated = true;
  }
  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, matador::time &)
{
  if (info_[index].buffer == nullptr) {
    size_t s = sizeof(MYSQL_TIME);
    info_[index].buffer = new char[s];
    memset(info_[index].buffer, 0, s);
    info_[index].buffer_length = (unsigned long)s;
    info_[index].is_allocated = true;
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

void mysql_prepared_result::prepare_bind_column(int index, enum_field_types type, std::string & /*value*/, size_t s)
{
  if (info_[index].buffer == nullptr) {
    info_[index].buffer = new char[s];
    memset(info_[index].buffer, 0, s);
    info_[index].buffer_length = (unsigned long)s;
    info_[index].is_allocated = true;
  }
  bind_[index].buffer_type = type;
  bind_[index].buffer = info_[index].buffer;
  bind_[index].buffer_length = info_[index].buffer_length;
  bind_[index].buffer = nullptr;
  bind_[index].buffer_length = 0;
  bind_[index].is_null = &info_[index].is_null;
  bind_[index].length = &info_[index].length;
  bind_[index].error = &info_[index].error;
}

}
}
