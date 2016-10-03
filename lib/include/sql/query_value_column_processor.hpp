#ifndef OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP
#define OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP

#include "tools/varchar.hpp"
#include "tools/time.hpp"
#include "tools/any_visitor.hpp"

#include "sql/column.hpp"

namespace oos {
namespace detail {

class query_value_column_processor
{
public:
  query_value_column_processor(const std::shared_ptr<columns> &update_columns, const std::vector<oos::any> &rowvalues);

  void execute(std::pair<std::string, oos::any> &a);

  void process_char(char &val);
  void process_short(short &val);
  void process_int(int &val);
  void process_long(long &val);
  void process_uchar(unsigned char &val);
  void process_ushort(unsigned short &val);
  void process_uint(unsigned int &val);
  void process_ulong(unsigned long &val);
  void process_bool(bool &val);
  void process_float(float &val);
  void process_double(double &val);
  void process_charptr(char *val);
  void process_string(std::string &val);
  void process_varchar(oos::varchar_base &val);
  void process_time(oos::time &val);
  void process_date(oos::date &val);
private:
  any_visitor visitor_;
  std::shared_ptr<columns> update_columns_;
  std::vector<oos::any> rowvalues_;
  std::string current_id_;
};

}
}
#endif //OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP
