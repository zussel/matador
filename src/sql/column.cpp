#include <utility>

#include "matador/sql/column.hpp"
#include "matador/sql/token_visitor.hpp"

namespace matador {

column::column(std::string col, const field_attributes &attr)
  : token(COLUMN)
  , name(std::move(col))
  , val(any{})
  , attributes(attr)
{}

column::column(std::string col, t_build_options options, const field_attributes &attr)
  : token(COLUMN)
  , name(std::move(col))
  , build_options(options)
  , val(any{})
  , attributes(attr)
{}

void column::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

column::column(std::string name, const matador::any& val, const field_attributes &attr)
  : token(COLUMN)
  , name(std::move(name))
  , val(val)
  , attributes(attr) {}

column operator "" _col(const char *name, size_t len)
{
    return column(std::string(name, len));
}

std::shared_ptr<column> make_column(const std::string &name, data_type type, const any &val, t_build_options options, size_t index, const field_attributes &attr)
{
  auto col = std::make_shared<column>(name, val, attr);
  col->index = index;
  col->type = type;
  col->build_options = options;

  return col;
}

std::shared_ptr<column> make_pk_column(const std::string &name, data_type type, size_t index, size_t max_size)
{
  return make_column(name, type, index, { max_size, constraints::PRIMARY_KEY });
}

std::shared_ptr<column> make_column(const std::string &name, data_type type, size_t index, const field_attributes &attr)
{
  auto col = std::make_shared<column>(name, attr);
  col->type = type;
  col->index = index;
  col->build_options |= t_build_options::with_type;
  return col;
}

}
