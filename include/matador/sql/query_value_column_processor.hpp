#ifndef OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP
#define OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP

#include "matador/utils/time.hpp"

#include "matador/sql/columns.hpp"

namespace matador::detail {

/// @cond MATADOR_DEV

class query_value_column_processor
{
public:
  explicit query_value_column_processor(std::shared_ptr<columns> update_columns);

  void execute(std::pair<std::string, sql::column_type> &a);

  void operator()(char &x) { process(x); }
  void operator()(short &x) { process(x); }
  void operator()(int &x) { process(x); }
  void operator()(long &x) { process(x); }
  void operator()(long long &x) { process(x); }
  void operator()(unsigned char &x) { process(x); }
  void operator()(unsigned short &x) { process(x); }
  void operator()(unsigned int &x) { process(x); }
  void operator()(unsigned long &x) { process(x); }
  void operator()(unsigned long long &x) { process(x); }
  void operator()(bool &x) { process(x); }
  void operator()(float &x) { process(x); }
  void operator()(double &x) { process(x); }
  void operator()(const char *x) { process(x); }
  void operator()(std::string &x) { process(x); }
  void operator()(matador::date &x) { process(x); }
  void operator()(matador::time &x) { process(x); }
  void operator()(utils::blob &x) { process(x); }
  void operator()(sql::placeholder &/*x*/) {}

private:
  template < class T >
  void process(T &val)
  {
    update_columns_->push_back(make_column(current_id_, val));
  }
  void process(char *val);
  void process(const char *val);
  void process(utils::blob &val);

private:
  std::shared_ptr<columns> update_columns_;
  sql::column_type column_value_;
  std::string current_id_;
};

/// @endcond

}

#endif //OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP
