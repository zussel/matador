#include "sql/query_value_column_processor.hpp"

namespace oos {
namespace detail {

query_value_column_processor::query_value_column_processor(
  const std::shared_ptr<columns> &update_columns,
  const std::vector<oos::any> &rowvalues
)
  : update_columns_(update_columns)
  , rowvalues_(rowvalues)
{
  visitor_.register_visitor<char>(std::bind(&query_value_column_processor::process_char, this, std::placeholders::_1));
  visitor_.register_visitor<short>(std::bind(&query_value_column_processor::process_short, this, std::placeholders::_1));
  visitor_.register_visitor<int>(std::bind(&query_value_column_processor::process_int, this, std::placeholders::_1));
  visitor_.register_visitor<long>(std::bind(&query_value_column_processor::process_long, this, std::placeholders::_1));
  visitor_.register_visitor<unsigned char>(std::bind(&query_value_column_processor::process_uchar, this, std::placeholders::_1));
  visitor_.register_visitor<unsigned short>(std::bind(&query_value_column_processor::process_ushort, this, std::placeholders::_1));
  visitor_.register_visitor<unsigned int>(std::bind(&query_value_column_processor::process_uint, this, std::placeholders::_1));
  visitor_.register_visitor<unsigned long>(std::bind(&query_value_column_processor::process_ulong, this, std::placeholders::_1));
  visitor_.register_visitor<bool>(std::bind(&query_value_column_processor::process_bool, this, std::placeholders::_1));
  visitor_.register_visitor<float>(std::bind(&query_value_column_processor::process_float, this, std::placeholders::_1));
  visitor_.register_visitor<double>(std::bind(&query_value_column_processor::process_double, this, std::placeholders::_1));
  visitor_.register_visitor<oos::varchar_base>(std::bind(&query_value_column_processor::process_varchar, this, std::placeholders::_1));
  visitor_.register_visitor<std::string>(std::bind(&query_value_column_processor::process_string, this, std::placeholders::_1));
  visitor_.register_visitor<char*>(std::bind(&query_value_column_processor::process_charptr, this, std::placeholders::_1));
  visitor_.register_visitor<oos::time>(std::bind(&query_value_column_processor::process_time, this, std::placeholders::_1));
  visitor_.register_visitor<oos::date>(std::bind(&query_value_column_processor::process_date, this, std::placeholders::_1));
}

void query_value_column_processor::execute(std::pair<std::string, oos::any> &a)
{
  current_id_ = a.first;
  rowvalues_.push_back(a.second);
  visitor_.visit(rowvalues_.back());
}

template < class T >
void process_value(const std::shared_ptr<oos::columns> &columns, const std::string &id, const T &val)
{
  std::shared_ptr<detail::value_column<T>> ival(new detail::value_column<T>(id, val));
  columns->push_back(ival);
}

void process_value(const std::shared_ptr<oos::columns> &columns, const std::string &id, const char *val, size_t len)
{
  std::shared_ptr<detail::value_column<const char*>> ival(new detail::value_column<const char*>(id, val, len));
  columns->push_back(ival);
}

void query_value_column_processor::process_char(char &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_short(short &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_int(int &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_long(long &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_uchar(unsigned char &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_ushort(unsigned short &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_uint(unsigned int &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_ulong(unsigned long &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_bool(bool &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_float(float &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_double(double &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_charptr(char *val)
{
  process_value(update_columns_, current_id_, val, strlen(val));
}

void query_value_column_processor::process_string(std::string &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_varchar(oos::varchar_base &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_time(oos::time &val)
{
  process_value(update_columns_, current_id_, val);
}

void query_value_column_processor::process_date(oos::date &val)
{
  process_value(update_columns_, current_id_, val);
}
}
}