#ifndef MATADOR_DIRECTORS_PAGE_HPP
#define MATADOR_DIRECTORS_PAGE_HPP

#include "crud_page.hpp"

#include "../models/person.hpp"

#include "matador/orm/persistence.hpp"

#include "matador/http/response.hpp"

#include "matador/logger/logger.hpp"

class directors_page : public crud_page<person>
{
public:
  directors_page(matador::http::server &s, matador::persistence &p);

protected:
  void prepare_json_form_data(matador::json &/*data*/) override {}
};


#endif //MATADOR_DIRECTORS_PAGE_HPP
