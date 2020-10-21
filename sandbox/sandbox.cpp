#include "matador/utils/stream.hpp"

#include <string>
#include <iostream>

using namespace matador;

bool is_even(int i) { return i % 2 == 0; }
bool is_odd(int i) { return i % 2 != 0; }
std::string to_string(int i) { return typeid(i).name() + std::to_string(i); }

struct person
{
  person(std::string n, int a) : name(std::move(n)), age(a) {}
  std::string name;
  int age = 0;
};

int main()
{
  std::list<int> ints = { 1, 2, 3, 4 };

  auto range = detail::make_from<int>(std::begin(ints), std::end(ints));

  auto filter = make_filter([](const int &in) {
    return in % 2 == 0;
  }, range);

  auto mapper = detail::make_mapper([](const int &in) {
    return typeid(in).name() + std::to_string(in);
  }, filter);

  mapper->process();
  std::cout << "Mapper: " << *mapper->value() << "\n";

  auto s = make_stream(ints);

  auto l = s.to_vector();

  for (const auto &i : l) {
    std::cout << "i: " << i << "\n";
  }

  std::cout << "done\n";

  auto s2 = s
    .filter(is_even)
    .map(to_string);

  auto result = s2.to_vector();

  for (const auto &i : s2) {
    std::cout << "i: " << i << "\n";
  }

  std::cout << "done\n";

  make_stream(ints)
    .take(2)
    .print_to(std::cout, " (take)\n");

  std::cout << "done\n";

  make_stream(ints)
    .skip(2)
    .print_to(std::cout, " (skip)\n");

  std::cout << "done\n";

  make_stream(ints)
    .skip_while([](const int &i) { return i > 1 && i < 4; })
    .print_to(std::cout, " (skip while)\n");

  std::cout << "done\n";

  make_stream(ints)
    .take_while([](const int &i) { return i > 1 && i < 4; })
    .print_to(std::cout, " (take while)\n");

  std::cout << "done\n";

  std::vector<person> persons = {
    { "otto", 34 },
    { "jane", 27 },
    { "george", 56 },
    { "bobby", 15 }
  };

  make_stream(persons)
    .map([](const person &p) { return p.age; })
    .print_to(std::cout, " (map age)\n");

  std::cout << "done\n";

  make_stream(3, 17)
    .print_to(std::cout, " (range)\n");

  std::cout << "count range(3, 17): " << make_stream(3, 17).count() << "\n";

  make_stream(3, 17, 2)
  .print_to(std::cout, " (range 2)\n");

  std::cout << "count range(3, 17, 2): " << make_stream(3, 17, 2).count() << "\n";

  std::cout << "done\n";

  make_stream_counter(6)
    .filter(is_even)
    .take(10)
    .print_to(std::cout, " (counter)\n");

  std::cout << "done\n";

  make_stream_counter(6, 4)
    .filter(is_even)
    .take(10)
    .print_to(std::cout, " (counter)\n");

  std::cout << "any even in range(1, 6): " << std::boolalpha << make_stream(1, 6).any(is_even) << "\n";
  std::cout << "all less 7 range(1, 6): " << std::boolalpha << make_stream(1, 6).all([](const int &val) { return val < 7; }) << "\n";
  std::cout << "all less 7 range(1, 8): " << std::boolalpha << make_stream(1, 8).all([](const int &val) { return val < 7; }) << "\n";

  return 0;
}