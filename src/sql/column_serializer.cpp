#include "matador/sql/column_serializer.hpp"

namespace matador {
namespace detail {

column_serializer::column_serializer(columns::t_brackets brackets)
  : brackets_(brackets)
{}

void column_serializer::on_primary_key(const char *id, std::string &, size_t /*size*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_revision(const char *id, unsigned long long int &/*rev*/)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_attribute(const char *id, char * /*x*/, const field_attributes &)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_belongs_to(const char *id, identifiable_holder &, cascade_type)
{
  cols_->push_back(std::make_shared<column>(id));
}

void column_serializer::on_has_one(const char *id, identifiable_holder &, cascade_type)
{
  cols_->push_back(std::make_shared<column>(id));
}

}
}
