#include "mysql_result_binder.hpp"

namespace matador::backends::mysql {

namespace detail {

template < class T >
void prepare_bind_column(MYSQL_BIND &binding, mysql_result_info &result_info, enum_field_types type, T &value)
{
  binding.buffer= (char *)&value;
  binding.buffer_length = sizeof(T);
  binding.buffer_type = type;
  binding.is_null = &result_info.is_null;
  binding.length = &result_info.length;
  binding.error = &result_info.error;
}

void prepare_bind_column(MYSQL_BIND &binding, mysql_result_info &result_info, enum_field_types type, matador::date &)
{
  result_info.initialize(sizeof(MYSQL_TIME));
  binding.buffer_type = type;
  binding.buffer = result_info.buffer.data();
  binding.buffer_length = result_info.buffer_size;
  binding.is_null = &result_info.is_null;
  binding.length = &result_info.length;
  binding.error = &result_info.error;
}

void prepare_bind_column(MYSQL_BIND &binding, mysql_result_info &result_info, enum_field_types type, matador::time &)
{
  result_info.initialize(sizeof(MYSQL_TIME));
  binding.buffer_type = type;
  binding.buffer = result_info.buffer.data();
  binding.buffer_length = result_info.buffer_size;
  binding.is_null = &result_info.is_null;
  binding.length = &result_info.length;
  binding.error = &result_info.error;
}

void prepare_bind_column(MYSQL_BIND &binding, mysql_result_info &result_info, enum_field_types type, char *x, size_t s)
{
  binding.buffer_type = type;
  binding.buffer= x;
  binding.buffer_length = (unsigned long) s;
  binding.is_null = &result_info.is_null;
  binding.length = &result_info.length;
  binding.error = &result_info.error;
}

void prepare_bind_column(MYSQL_BIND &binding, mysql_result_info &result_info, enum_field_types type, std::string &/*value*/)
{
  binding.buffer_type = type;
  binding.buffer = nullptr;
  binding.buffer_length = 0;
  binding.is_null = &result_info.is_null;
  binding.length = &result_info.length;
  binding.error = &result_info.error;
}

void prepare_bind_column(MYSQL_BIND &binding, mysql_result_info &result_info, enum_field_types type, std::string & /*value*/, size_t s)
{
  result_info.initialize(s);
  binding.buffer_type = type;
  binding.buffer = result_info.buffer.data();
  binding.buffer_length = result_info.buffer_size;
  binding.is_null = &result_info.is_null;
  binding.length = &result_info.length;
  binding.error = &result_info.error;
}

void prepare_bind_column(MYSQL_BIND &binding, mysql_result_info &result_info, enum_field_types type, utils::blob &/*value*/)
{
  result_info.initialize(1024);
  binding.buffer_type = type;
  binding.buffer = result_info.buffer.data();
  binding.buffer_length = result_info.buffer_size;
  binding.is_null = &result_info.is_null;
  binding.length = &result_info.length;
  binding.error = &result_info.error;
}

}

mysql_result_binder::mysql_result_binder(size_t size)
: result_bindings_(size)
, is_null_vector(size)
, result_infos_(size)
{

}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, char &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_TINY, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, short &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_SHORT, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, int &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_LONG, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, long &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_LONGLONG, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, long long int &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_LONGLONG, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, unsigned char &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_TINY, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, unsigned short &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_SHORT, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, unsigned int &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_LONG, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, unsigned long &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_LONGLONG, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, unsigned long long int &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_LONGLONG, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, bool &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_TINY, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, float &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_FLOAT, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, double &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_DOUBLE, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, time &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_DATE, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, date &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_TIMESTAMP, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, char *x, size_t size)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_VARCHAR, x, size);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, std::string &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_VARCHAR, x);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, std::string &x, size_t size)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_VAR_STRING, x, size);
}

void mysql_result_binder::read_value(const char * /*id*/, size_t pos, utils::blob &x)
{
  detail::prepare_bind_column(result_bindings_[pos], result_infos_[pos], MYSQL_TYPE_BLOB, x);
}

std::vector<MYSQL_BIND> &mysql_result_binder::result_bindings()
{
  return result_bindings_;
}

std::vector<mysql_result_info> &mysql_result_binder::result_infos()
{
  return result_infos_;
}
}