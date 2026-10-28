#ifndef QUERY_PLACEHOLDER_HPP
#define QUERY_PLACEHOLDER_HPP

namespace matador::query {

struct placeholder {};

constexpr bool operator==(const placeholder&, const placeholder&) { return true; }

static constexpr placeholder _;

}

#endif //QUERY_PLACEHOLDER_HPP
