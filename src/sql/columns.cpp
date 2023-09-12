#include "matador/sql/columns.hpp"

namespace matador {

columns::columns(const std::initializer_list<std::string> &column_names, t_brackets with_brackets)
: token(COLUMNS)
, with_brackets_(with_brackets)
{
  for (const auto &column_name : column_names) {
    push_back(std::make_shared<column>(column_name));
  }
}

columns::columns(std::initializer_list<column> cols, t_brackets with_brackets)
: token(COLUMNS)
, with_brackets_(with_brackets)
{
  for (auto &&col : cols) {
    push_back(std::make_shared<column>(col));
  }
}

columns::columns(std::initializer_list<std::shared_ptr<column>> cols, t_brackets with_brackets)
: token(COLUMNS)
, with_brackets_(with_brackets)
{
  columns_.assign(cols);
}

columns::columns(t_brackets with_brackets)
: token(COLUMNS)
, with_brackets_(with_brackets)
{}

columns::columns(const columns &x)
: token(x.type)
, with_brackets_(x.with_brackets_)
{
  columns_.assign(x.columns_.begin(), x.columns_.end());
}

columns &columns::operator=(const columns &x)
{
  type = x.type;
  with_brackets_ = x.with_brackets_;
  columns_.clear();
  columns_.assign(x.columns_.begin(), x.columns_.end());
  return *this;
}

void columns::push_back(const std::shared_ptr<column> &col)
{
  columns_.push_back(col);
}

columns &columns::with_brackets()
{
  with_brackets_ = WITH_BRACKETS;
  return *this;
}

columns &columns::without_brackets()
{
  with_brackets_ = WITHOUT_BRACKETS;
  return *this;
}

void columns::accept(token_visitor &visitor)
{
  return visitor.visit(*this);
}

columns columns::all()
{
  return all_;
}

column columns::count_all()
{
  return count_all_;
}

columns columns::all_ = columns({column("*", t_build_options::empty)}, WITHOUT_BRACKETS);
column columns::count_all_ = column("COUNT(*)", t_build_options::empty);

}