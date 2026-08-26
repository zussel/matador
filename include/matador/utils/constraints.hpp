#ifndef MATADOR_CONSTRAINTS_HPP
#define MATADOR_CONSTRAINTS_HPP

#include <cstdint>

namespace matador::utils {
enum class constraints : uint8_t {
  None =        0,
  Index =       1 << 0,
  Unique =      1 << 1,
  PrimaryKey =  1 << 2,
  ForeignKey =  1 << 3,
  Identity =    1 << 4,
  Default =     1 << 5,
  NotNull =     1 << 6
};

inline constraints operator|(constraints a, constraints b) { return static_cast<constraints>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b)); }
inline constraints operator&(constraints a, constraints b) { return static_cast<constraints>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b)); }
inline constraints& operator|= (constraints& a, constraints b) { return reinterpret_cast<constraints&>(reinterpret_cast<int&>(a) |= static_cast<int>(b)); }
inline constraints& operator&= (constraints& a, constraints b) { return reinterpret_cast<constraints&>(reinterpret_cast<int&>(a) &= static_cast<int>(b)); }

inline bool is_constraint_set(const constraints source, const constraints needle) { return static_cast<int>(source & needle) > 0; }
}

#endif //MATADOR_CONSTRAINTS_HPP
