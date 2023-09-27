#include "directors_page.hpp"

directors_page::directors_page(matador::http::server &server, matador::persistence &p)
  : crud_page<person>({"Director", "Directors", "director"}, server, p)
{}
