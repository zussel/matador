#ifndef QUERY_BOOK_HPP
#define QUERY_BOOK_HPP

#include "matador/object/object_ptr.hpp"

#include "matador/utils/access.hpp"
#include "matador/utils/foreign_attributes.hpp"

#include <string>

namespace matador::test {

struct author;

struct book
{
  unsigned long id{};
  std::string title;
  matador::object_ptr<author> book_author;
  unsigned short published_in{};

  template<typename Operator>
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "title", title, 511);
    field::belongs_to(op, "author_id", book_author, utils::fetch_type::LAZY);
    field::attribute(op, "published_in", published_in);
  }
};

}
#endif //QUERY_BOOK_HPP
