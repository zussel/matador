#include "mysql_prepared_result.hpp"
#include "serializer.hpp"

#include <ostream>

namespace oos {

namespace mysql {

mysql_prepared_result::mysql_prepared_result(MYSQL_STMT *s, MYSQL_BIND *b, int rs)
  : affected_rows_(mysql_stmt_affected_rows(s))
  , rows(mysql_stmt_num_rows(s))
  , fields_(mysql_stmt_field_count(s))
  , stmt(s)
  , result_index(0)
  , result_size(rs)
  , bind_(b)
{
}

mysql_prepared_result::~mysql_prepared_result()
{
}

void mysql_prepared_result::get(serializable *o)
{
  result_index = 0;
  o->deserialize(*this);
}

const char* mysql_prepared_result::column(size_type ) const
{
  return 0;
}

bool mysql_prepared_result::fetch()
{
  // get next row
  mysql_stmt_fetch(stmt);
    
  return rows-- > 0;
}

mysql_prepared_result::size_type mysql_prepared_result::affected_rows() const
{
  return affected_rows_;
}

mysql_prepared_result::size_type mysql_prepared_result::result_rows() const
{
  return rows;
}

mysql_prepared_result::size_type mysql_prepared_result::fields() const
{
  return fields_;
}

void mysql_prepared_result::read(const char *, long &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_LONG, x);
}
void mysql_prepared_result::read(const char *, double &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_DOUBLE, x);
}
void mysql_prepared_result::read(const char *, std::string &x)
{
  get_column_value(bind_[result_index++], MYSQL_TYPE_DOUBLE, x);
}

void mysql_prepared_result::get_column_value(MYSQL_BIND &bind, enum_field_types , std::string &value)
{
  value.assign((const char*)bind.buffer/*, (std::string::size_type)bind.length*/);
}

std::ostream& operator<<(std::ostream &out, const mysql_prepared_result &res)
{  
  out << "affected rows [" << res.affected_rows_ << "] size [" << res.rows << "]";
  return out;
}

}

}
