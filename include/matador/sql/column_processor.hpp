#ifndef MATADOR_COLUMN_PROCESSOR_HPP
#define MATADOR_COLUMN_PROCESSOR_HPP

#include <matador/utils/singleton.hpp>

#include <matador/sql/columns.hpp>
#include <matador/sql/column_serializer.hpp>
#include <matador/sql/query_value_column_processor.hpp>
#include <matador/sql/query_value_creator.hpp>
#include <matador/sql/typed_column_serializer.hpp>
#include <matador/sql/value_serializer.hpp>
#include <matador/sql/value_column_serializer.hpp>

namespace matador {

class column_processor : public singleton<column_processor>
{
public:
  template<class Type>
  std::unique_ptr<columns> to_column_names(Type &obj, columns::t_brackets brackets)
  {
    return column_serializer_.execute(obj, brackets);
  }

  template<class Type>
  std::unique_ptr<columns> to_update_pairs(Type &obj, columns::t_brackets brackets)
  {
    return value_column_serializer_.execute(obj, brackets);
  }

  std::unique_ptr<columns> to_update_pairs(const std::vector<std::pair<std::string, matador::any>> &column_values)
  {
    return query_value_column_processor_.execute(column_values);
  }

  template<class Type>
  std::unique_ptr<columns> to_create_columns(Type &obj)
  {
    return typed_column_serializer_.execute(obj);
  }

  std::unique_ptr<value> to_value(any &a)
  {
    return query_value_creator_.create_from_any(a);
  }

  template<class Type>
  std::unique_ptr<detail::values> to_values(Type &obj)
  {
    return value_serializer_.execute(obj);
  }

private:
  friend class singleton<column_processor>;

  column_processor() = default;

private:
  detail::column_serializer column_serializer_;
  detail::query_value_column_processor query_value_column_processor_;
  detail::query_value_creator query_value_creator_;
  detail::typed_column_serializer typed_column_serializer_;
  detail::value_serializer value_serializer_;
  detail::value_column_serializer value_column_serializer_;
};

}

#endif //MATADOR_COLUMN_PROCESSOR_HPP
