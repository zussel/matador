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

template<const query::primary_key_options &PkAttribute>
struct book_pk_generator;

template<const query::primary_key_options &PkAttribute>
struct author_pk_generator {
  unsigned int id{};
  std::string name;
  query::collection<query::object_ptr<book_pk_generator<PkAttribute>>> books;

  author_pk_generator() = default;
  explicit author_pk_generator(std::string name)
  : name(std::move(name)) {}

  template<typename Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id, PkAttribute);
    field::attribute(op, "name", name, VarChar63);
    field::has_many(op, "books", books, "author_id", CascadeAllFetchLazy);
  }
};

template<const query::primary_key_options &PkAttribute>
struct book_pk_generator {
  unsigned int id{};
  std::string title;
  query::object_ptr<author_pk_generator<PkAttribute>> book_author;
  unsigned short published_in{};

  book_pk_generator() = default;
  book_pk_generator(std::string title, query::object_ptr<author_pk_generator<PkAttribute>> author, const unsigned short published_in)
  : title(std::move(title)), book_author(std::move(author)), published_in(published_in) {}

  template<typename Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id, PkAttribute);
    field::attribute(op, "title", title, VarChar511);
    field::belongs_to(op, "author_id", book_author, CascadeAllFetchEager);
    field::attribute(op, "published_in", published_in);
  }
};

using author_identity = author_pk_generator<query::Identity>;
using author_sequence = author_pk_generator<query::Sequence>;
using author_table = author_pk_generator<query::Table>;
using book_identity = book_pk_generator<query::Identity>;
using book_sequence = book_pk_generator<query::Sequence>;
using book_table = book_pk_generator<query::Table>;
}

#endif //QUERY_AUTHOR_HPP
