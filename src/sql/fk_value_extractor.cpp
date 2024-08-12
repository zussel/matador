#include "matador/sql/fk_value_extractor.hpp"

namespace matador::sql::detail {

void fk_value_extractor::on_primary_key(const char *, std::string &pk, size_t)
{
  value_ = pk;
}

}