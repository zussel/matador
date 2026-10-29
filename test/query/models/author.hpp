#ifndef QUERY_AUTHOR_HPP
#define QUERY_AUTHOR_HPP

#include "matador/query/relations/object_ptr.hpp"
#include "matador/query/relations/collection.hpp"

#include "matador/query/mapping/access.hpp"
#include "matador/query/model/column_options.hpp"
#include "matador/query/model/foreign_key_options.hpp"

#include <string>

namespace matador::test {
struct book;

struct author {
  author() = default;
  author(const unsigned int id, std::string first_name, std::string last_name, std::string date_of_birth, const unsigned short year_of_birth, const bool distinguished)
  : id(id), first_name(std::move(first_name))
  , last_name(std::move(last_name))
  , date_of_birth(std::move(date_of_birth))
  , year_of_birth(year_of_birth)
  , distinguished(distinguished) {}
  unsigned int id{};
  std::string first_name;
  std::string last_name;
  std::string date_of_birth;
  unsigned short year_of_birth{};
  bool distinguished{false};
  query::collection<query::object_ptr<book> > books;

  template<typename Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "first_name", first_name, VarChar63);
    field::attribute(op, "last_name", last_name, VarChar63);
    field::attribute(op, "date_of_birth", date_of_birth, VarChar63);
    field::attribute(op, "year_of_birth", year_of_birth);
    field::attribute(op, "distinguished", distinguished);
    field::has_many(op, "books", books, "author_id", CascadeAllFetchLazy);
  }
};
}

#endif //QUERY_AUTHOR_HPP
