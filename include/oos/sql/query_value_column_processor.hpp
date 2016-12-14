#ifndef OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP
#define OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP

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

#include "oos/utils/varchar.hpp"
#include "oos/utils/time.hpp"
#include "oos/utils/any_visitor.hpp"

#include "oos/sql/column.hpp"

namespace oos {
namespace detail {

/// @cond OOS_DEV

class OOS_API query_value_column_processor
{
public:
  query_value_column_processor(const std::shared_ptr<columns> &update_columns, const std::vector<oos::any> &rowvalues);

  void execute(std::pair<std::string, oos::any> &a);

private:
  template < class T >
  void process(T &val)
  {
    std::shared_ptr<detail::value_column<T>> ival(new detail::value_column<T>(current_id_, val));
    update_columns_->push_back(ival);
  }
  void process(char *val);
  void process(const char *val);

private:
  any_visitor visitor_;
  std::shared_ptr<columns> update_columns_;
  std::vector<oos::any> rowvalues_;
  std::string current_id_;
};

/// @endcond

}
}
#endif //OOS_QUERY_VALUE_COLUMN_PROCESSOR_HPP
