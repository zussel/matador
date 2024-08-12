#include "matador/sql/placeholder_key_value_generator.hpp"

namespace matador::sql {

void placeholder_key_value_generator::on_primary_key(const char *id, std::string &, size_t)
{
  placeholder_values.emplace_back(id, utils::_);
}

void placeholder_key_value_generator::on_revision(const char *id, unsigned long long int &)
{
  placeholder_values.emplace_back(id, utils::_);
}

}