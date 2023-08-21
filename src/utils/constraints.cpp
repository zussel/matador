#include "matador/utils/contraints.hpp"

namespace matador {

std::unordered_map<constraints, std::string> constraints_to_name_map() {
  std::unordered_map<constraints, std::string> m {
    { constraints::NONE, "none" },
    { constraints::NOT_NULL, "not null" },
    { constraints::INDEX, "index" },
    { constraints::UNIQUE, "unique" },
    { constraints::PRIMARY_KEY, "primary key" },
    { constraints::FOREIGN_KEY, "foreign key" },
    { constraints::DEFAULT, "default" }
  };
  return m;
}

}