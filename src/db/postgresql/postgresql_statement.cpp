//
// Created by sascha on 03.06.19.
//

#include "matador/db/postgresql/postgresql_statement.hpp"

namespace matador {

namespace postgresql {

postgresql_statement::postgresql_statement(postgresql_connection &db, const std::string &stmt)
  : db_(db)
{
  str(stmt);

}

postgresql_statement::~postgresql_statement()
{

}

void postgresql_statement::clear()
{

}

detail::result_impl *postgresql_statement::execute()
{
  return nullptr;
}

void postgresql_statement::reset()
{

}

void postgresql_statement::serialize(const char *id, char &x)
{

}

void postgresql_statement::serialize(const char *id, short &x)
{

}

void postgresql_statement::serialize(const char *id, int &x)
{

}

void postgresql_statement::serialize(const char *id, long &x)
{

}

void postgresql_statement::serialize(const char *id, unsigned char &x)
{

}

void postgresql_statement::serialize(const char *id, unsigned short &x)
{

}

void postgresql_statement::serialize(const char *id, unsigned int &x)
{

}

void postgresql_statement::serialize(const char *id, unsigned long &x)
{

}

void postgresql_statement::serialize(const char *id, float &x)
{

}

void postgresql_statement::serialize(const char *id, double &x)
{

}

void postgresql_statement::serialize(const char *id, bool &x)
{

}

void postgresql_statement::serialize(const char *id, char *x, size_t s)
{

}

void postgresql_statement::serialize(const char *id, varchar_base &x)
{

}

void postgresql_statement::serialize(const char *id, std::string &x)
{

}

void postgresql_statement::serialize(const char *id, matador::date &x)
{

}

void postgresql_statement::serialize(const char *id, matador::time &x)
{

}

void postgresql_statement::serialize(const char *id, basic_identifier &x)
{

}

void postgresql_statement::serialize(const char *id, identifiable_holder &x, cascade_type)
{

}
}
}