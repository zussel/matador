#include "matador/sql/table.hpp"

namespace matador::sql {
table::table(const char *name, std::string as)
: name(name)
, alias(std::move(as))
{}

table::table(std::string name, std::string as)
: name(std::move(name))
, alias(std::move(as))
{}

table & table::as(const std::string &a) {
  alias = a;
  return *this;
}

table table::as(const std::string &a) const {
  return { name, a, columns };
}

table operator ""_tab(const char *name, size_t len) {
  return {{name, len}};
}

}
