#include "matador/sql/placeholder_generator.hpp"

namespace matador::sql {

void placeholder_generator::on_primary_key(const char * /*id*/, std::string &, size_t)
{
  placeholder_values.emplace_back(utils::_);
}

void placeholder_generator::on_revision(const char * /*id*/, unsigned long long int &)
{
  placeholder_values.emplace_back(utils::_);
}

}