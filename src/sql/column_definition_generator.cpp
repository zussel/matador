#include "matador/sql/column_definition_generator.hpp"
#include "matador/sql/schema.hpp"

namespace matador::sql {

column_definition_generator::column_definition_generator(std::vector<column_definition> &columns, const schema &repo)
: columns_(columns)
, repo_(repo)
{}

void column_definition_generator::on_primary_key(const char *id, std::string &pk, size_t size)
{
  on_attribute(id, pk, { size, utils::constraints::PRIMARY_KEY });
}

void column_definition_generator::on_revision(const char *id, unsigned long long int &x)
{
  on_attribute(id, x);
}

void column_definition_generator::on_attribute(const char *id, const char *x, const utils::field_attributes &attr)
{
  columns_.emplace_back(id, x, attr, null_option::NOT_NULL);
}

void column_definition_generator::on_attribute(const char *id, std::optional<const char *> /*x*/, const utils::field_attributes &attr)
{
  columns_.emplace_back(id, object::data_type_traits<const char*>::type(attr.size()), attr, null_option::NULLABLE);
}

std::pair<std::string, std::string> column_definition_generator::determine_foreign_ref(const std::type_index &ti)
{
  return repo_.reference(ti);
}

void fk_column_generator::on_primary_key(const char *, std::string &, size_t size)
{
  type_ = object::data_type_traits<std::string>::type(size);
}

}