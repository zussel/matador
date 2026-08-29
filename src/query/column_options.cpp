#include "matador/query/column_options.hpp"

namespace matador::query {
column_options::column_options(const std::size_t size)
  : size_(size)
{}

column_options::column_options(const column_constraints constraints)
  : constraints_(constraints)
{}

column_options::column_options(const std::size_t size, const column_constraints constraints)
  : size_(size), constraints_(constraints)
{}

column_options& column_options::operator=(const std::size_t size) {
    size_ = size;
    constraints_ = column_constraint::None;
    return *this;
}
column_options& column_options::operator=(const column_constraints constraints) {
    constraints_ = constraints;
    size_ = 0;
    return *this;
}

std::size_t column_options::size() const {
    return size_;
}

column_constraints column_options::constraints() const {
    return constraints_;
}
}