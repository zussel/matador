#ifndef QUERY_AUTHOR_HPP
#define QUERY_AUTHOR_HPP

#include "matador/utils/access.hpp"

#include "matador/object/container.hpp"

#include <string>

namespace matador::test {

struct book;

struct author
{
  unsigned long id{};
  std::string first_name;
  std::string last_name;
  std::string date_of_birth;
  unsigned short year_of_birth{};
  bool distinguished{false};
  container<book> books;

  template<typename Operator>
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "first_name", first_name, 63);
    field::attribute(op, "last_name", last_name, 63);
    field::attribute(op, "date_of_birth", date_of_birth, 31);
    field::attribute(op, "year_of_birth", year_of_birth);
    field::attribute(op, "distinguished", distinguished);
    field::has_many(op, "books", books, "author_id", utils::fetch_type::LAZY);
  }
};

}

#endif //QUERY_AUTHOR_HPP
