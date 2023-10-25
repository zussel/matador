#ifndef OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP
#define OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP

#include "matador/utils/time.hpp"
#include "matador/utils/any_visitor.hpp"

#include "matador/sql/columns.hpp"

namespace matador {
namespace detail {

/// @cond MATADOR_DEV

class query_value_column_processor
{
public:
  query_value_column_processor();

  std::unique_ptr<columns> execute(const std::vector<std::pair<std::string, matador::any>> &column_values);

private:
  template < class T >
  void process(T &val)
  {
    update_columns_->push_back(make_column(current_id_, val));
  }
  void process(char *val);
  void process(const char *val);

private:
  any_visitor visitor_;
  std::unique_ptr<columns> update_columns_;
  std::vector<matador::any> row_values_;
  std::string current_id_;
};

/// @endcond

}
}
#endif //OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP
