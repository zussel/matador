#include "matador/utils/stream.hpp"

#include <string>
#include <iostream>

using namespace matador;

bool is_even(int i) { return i % 2 == 0; }
std::string to_string(int i) { return typeid(i).name() + std::to_string(i); }

int main()
{
  std::list<int> ints { 1, 2, 3, 4, };

  auto range = make_range<int>(std::begin(ints), std::end(ints));

  auto filter = make_filter([](const int &in) {
    return in % 2 == 0;
  }, range);

  auto mapper = make_mapper([](const int &in) {
    return typeid(in).name() + std::to_string(in);
  }, filter);

  mapper->process();
  std::cout << "Mapper: " << *mapper->value() << "\n";

  auto s = make_stream(ints);

  auto s2 = s
    .filter(is_even)
    .map(to_string);

  auto result = s2.to_vector();

  return 0;
}