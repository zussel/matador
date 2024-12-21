#ifndef ODBC_BOUND_VALUE_HPP
#define ODBC_BOUND_VALUE_HPP

#include <vector>

namespace matador::backends::odbc {

struct odbc_bound_value {
    explicit odbc_bound_value(const size_t size)
    : data(size) {};
    std::vector<unsigned char> data;
};

}

#endif //ODBC_BOUND_VALUE_HPP
