#include "database/mssql/mssql_prepared_result.hpp"

#include "object/object_atomizable.hpp"
#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"

#include <ostream>
#include <cstring>

namespace oos {

namespace mssql {

mssql_prepared_result::mssql_prepared_result(int rs)
  : affected_rows_(0)
  , rows(0)
  , fields_(0)
  , result_size(rs)
{
}

mssql_prepared_result::~mssql_prepared_result()
{
}

const char* mssql_prepared_result::column(size_type ) const
{
  return "not implemented";
}

bool mssql_prepared_result::fetch()
{    
  return rows-- > 0;
}

mssql_prepared_result::size_type mssql_prepared_result::affected_rows() const
{
  return affected_rows_;
}

mssql_prepared_result::size_type mssql_prepared_result::result_rows() const
{
  return rows;
}

mssql_prepared_result::size_type mssql_prepared_result::fields() const
{
  return fields_;
}

void mssql_prepared_result::read(const char *, char &x)
{
}

void mssql_prepared_result::read(const char *, short &x)
{
}

void mssql_prepared_result::read(const char *, int &x)
{
}

void mssql_prepared_result::read(const char *, long &x)
{
}

void mssql_prepared_result::read(const char *, unsigned char &x)
{
}

void mssql_prepared_result::read(const char *, unsigned short &x)
{
}

void mssql_prepared_result::read(const char *, unsigned int &x)
{
}

void mssql_prepared_result::read(const char *, unsigned long &x)
{
}

void mssql_prepared_result::read(const char *, bool &x)
{
}

void mssql_prepared_result::read(const char *, float &x)
{
}

void mssql_prepared_result::read(const char *, double &x)
{
}

void mssql_prepared_result::read(const char *, char *x, int s)
{
}

void mssql_prepared_result::read(const char *, std::string &x)
{
}

void mssql_prepared_result::read(const char *, varchar_base &x)
{
}

void mssql_prepared_result::read(const char *, object_base_ptr &x)
{
}

void mssql_prepared_result::read(const char *, object_container &)
{}

std::ostream& operator<<(std::ostream &out, const mssql_prepared_result &res)
{  
  out << "affected rows [" << res.affected_rows_ << "] size [" << res.rows << "]";
  return out;
}

}

}
