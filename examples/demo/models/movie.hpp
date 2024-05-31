#ifndef MATADOR_MOVIE_HPP
#define MATADOR_MOVIE_HPP

#include "matador/utils/cascade_type.hpp"

#include "matador/object/container.hpp"

#include <string>
#include <utility>

#include "person.hpp"

struct movie
{
  enum e_genre {
    ACTION,
    ROMANCE,
    COMEDY,
    SCIENCE_FICTION,
    HORROR,
    THRILLER
  };

  movie() = default;
  movie(std::string t, unsigned short y, const matador::object_ptr<person>& dir)
    : title(std::move(t)), year(y), director(dir)
  {}

  unsigned long id{};
  std::string title;
  matador::container<e_genre> genres;
  unsigned short year {};
  matador::container<person> actors;
  matador::object_ptr<person> director;

  template < class Operator >
  void process(Operator &op)
  {
    namespace field = matador::access;
    field::primary_key(op, "id", id);
    field::attribute(op, "title", title, 255);
    //serializer.serialize("genres", genres, "movie_id", "genre", matador::cascade_type::ALL);
    field::attribute(op, "year", year);
    field::has_many(op, "actors", actors, "movie_id", "actor_id", matador::cascade_type::NONE);
    field::has_one(op, "director", director, matador::cascade_type::NONE);
  }
};
#endif //MATADOR_MOVIE_HPP
