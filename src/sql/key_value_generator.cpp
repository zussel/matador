#include "matador/sql/key_value_generator.hpp"

#include "matador/sql/value_extractor.hpp"

namespace matador::sql {
void key_value_generator::on_primary_key(const char *id, std::string &x, size_t)
{
  result_.emplace_back(id, x);
}

void key_value_generator::on_revision(const char *id, unsigned long long int &x)
{
  result_.emplace_back(id, x);
}

}