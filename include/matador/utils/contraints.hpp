#ifndef MATADOR_CONTRAINTS_HPP
#define MATADOR_CONTRAINTS_HPP

#include <string>
#include <unordered_map>

namespace matador {

enum class constraints : unsigned char {
  NONE =        0,
  NOT_NULL =    1 << 0,
  INDEX =       1 << 1,
  UNIQUE =      1 << 2,
  PRIMARY_KEY = 1 << 3,
  FOREIGN_KEY = 1 << 4,
  DEFAULT =     1 << 5,
  UNIQUE_NOT_NULL = UNIQUE | NOT_NULL
};

static std::unordered_map<constraints, std::string> constraints_to_name_map();

inline constraints operator|(constraints a, constraints b)
{
  return static_cast<constraints>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline constraints operator&(constraints a, constraints b)
{
  return static_cast<constraints>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

inline bool is_constraint_set(constraints source, constraints needle)
{
  return static_cast<int>(source & needle) > 0;
}

}

#endif //MATADOR_CONTRAINTS_HPP
