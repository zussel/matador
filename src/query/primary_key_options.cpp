#include "matador/query/primary_key_options.hpp"

namespace matador::query {
primary_key_options::primary_key_options(const size_t size)
: size_( size ) {}

primary_key_options::primary_key_options(const generator_type generator)
: generator_( generator ) {}

primary_key_options::primary_key_options(const size_t size, const generator_type generator)
: size_( size )
, generator_( generator ) {}

primary_key_options& primary_key_options::operator=(const size_t size) {
  size_ = size;
  return *this;
}

primary_key_options& primary_key_options::operator=(const generator_type generator) {
  generator_ = generator;
  return *this;
}

size_t primary_key_options::size() const {
  return size_;
}

generator_type primary_key_options::generator() const {
  return generator_;
}

}