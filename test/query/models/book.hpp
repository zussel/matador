#ifndef QUERY_BOOK_HPP
#define QUERY_BOOK_HPP

#include "matador/query/relations/object_ptr.hpp"

#include "matador/query/mapping/access.hpp"
#include "matador/query/model/column_options.hpp"
#include "matador/query/model/foreign_key_options.hpp"

#include <string>

namespace matador::test {
struct author;

struct book {
  book() = default;
  book(const unsigned int id, std::string title, query::object_ptr<author> author, const unsigned short published_in)
  : id(id), title(std::move(title)), book_author(std::move(author)), published_in(published_in) {}
  unsigned int id{};
  std::string title;
  query::object_ptr<author> book_author;
  unsigned short published_in{};

  template<typename Operator>
  void process(Operator &op) {
    namespace field = query::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "title", title, VarChar511);
    field::belongs_to(op, "author_id", book_author, CascadeAllFetchEager);
    field::attribute(op, "published_in", published_in);
  }
};
}
#endif //QUERY_BOOK_HPP
