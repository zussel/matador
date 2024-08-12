#ifndef QUERY_PLACEHOLDER_HPP
#define QUERY_PLACEHOLDER_HPP

namespace matador::utils {

struct placeholder {};

inline constexpr bool operator==(const placeholder&, const placeholder&) { return true; }

static constexpr placeholder _;

}

#endif //QUERY_PLACEHOLDER_HPP
