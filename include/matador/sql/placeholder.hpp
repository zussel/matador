#ifndef MATADOR_PLACEHOLDER_HPP
#define MATADOR_PLACEHOLDER_HPP

namespace matador::sql {

struct placeholder {};

inline constexpr bool operator==(const placeholder&, const placeholder&) { return true; }

const placeholder _;

}

#endif //MATADOR_PLACEHOLDER_HPP
