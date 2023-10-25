#include "matador/sql/column_serializer.hpp"

namespace matador {
namespace detail {

void column_serializer::on_primary_key(const char *id, std::string &, size_t /*size*/)
{
  cols_->emplace_back(id);
}

void column_serializer::on_revision(const char *id, unsigned long long int &/*rev*/)
{
  cols_->emplace_back(id);
}

void column_serializer::on_attribute(const char *id, char * /*x*/, const field_attributes &)
{
  cols_->emplace_back(id);
}

void column_serializer::on_belongs_to(const char *id, identifiable_holder &, cascade_type)
{
  cols_->emplace_back(id);
}

void column_serializer::on_has_one(const char *id, identifiable_holder &, cascade_type)
{
  cols_->emplace_back(id);
}

}
}
