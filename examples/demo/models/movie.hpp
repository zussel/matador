#ifndef MATADOR_MOVIE_HPP
#define MATADOR_MOVIE_HPP

#include "matador/utils/cascade_type.hpp"

#include "matador/object/has_many.hpp"

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

  unsigned long id;
  std::string title;
  matador::has_many<e_genre> genres;
  unsigned short year {};
  matador::has_many<person> actors;
  matador::object_ptr<person> director;

  template < class Serializer >
  void serialize(Serializer &serializer)
  {
    serializer.on_primary_key("id", id);
    serializer.on_attribute("title", title, 255);
    //serializer.serialize("genres", genres, "movie_id", "genre", matador::cascade_type::ALL);
    serializer.on_attribute("year", year);
    serializer.on_has_many("actors", actors, "movie_id", "actor_id", matador::cascade_type::NONE);
    serializer.on_has_one("director", director, matador::cascade_type::NONE);
  }
};
#endif //MATADOR_MOVIE_HPP
