#include "database/sql.hpp"

namespace oos {

sql::~sql()
{
  reset();
}

void sql::append(const std::string &str)
{
  token_list_.push_back(new string_token(str));
}

void sql::append(const char *id, data_type_t type)
{
  /*
   * create new field, append it
   * to token list and field vector
   * and insert it into field map
   */
  field_ptr f(new field(id, type, result_field_vector_.size(), false));
  
  token_list_.push_back(new result_field_token(f));
  result_field_map_.insert(std::make_pair(id, f));
  result_field_vector_.push_back(f);
}

void sql::append(const char *id, data_type_t type, const std::string &val)
{
  /*
   * create new field, append it
   * to token list and field vector
   * and insert it into field map
   */
  field_ptr f(new field(id, type, host_field_vector_.size(), 1024));
  
  token_list_.push_back(new host_field_token(f, val));
  host_field_map_.insert(std::make_pair(id, f));
  host_field_vector_.push_back(f);
}

std::string sql::prepare() const
{
  return generate(true);
}

std::string sql::direct() const
{
  return generate(false);
}

void sql::reset()
{
  host_field_map_.clear();
  host_field_vector_.clear();
  result_field_map_.clear();
  result_field_vector_.clear();
  while (!token_list_.empty()) {
    delete token_list_.back();
    token_list_.pop_back();
  }
  token_list_.clear();
}

sql::iterator sql::result_begin()
{
  return result_field_vector_.begin();
}

sql::iterator sql::result_end()
{
  return result_field_vector_.end();
}

sql::const_iterator sql::result_begin() const
{
  return result_field_vector_.begin();
}

sql::const_iterator sql::result_end() const
{
  return result_field_vector_.end();
}

sql::size_type sql::result_size() const
{
  return result_field_vector_.size();
}

sql::iterator sql::host_begin()
{
  return host_field_vector_.begin();
}

sql::iterator sql::host_end()
{
  return host_field_vector_.end();
}

sql::const_iterator sql::host_begin() const
{
  return host_field_vector_.begin();
}

sql::const_iterator sql::host_end() const
{
  return host_field_vector_.end();
}

sql::size_type sql::host_size() const
{
  return host_field_vector_.size();
}

std::string sql::generate(bool prepared) const
{
  std::string str;
  token_list_t::const_iterator first = token_list_.begin();
  token_list_t::const_iterator last = token_list_.end();
  while (first != last) {
    const token *t = *first++;
    str += t->get(prepared);
  }
  return str;
}

unsigned int sql::type_size(data_type_t type)
{
  switch(type) {
    case type_char:
      return sizeof(char);
    case type_short:
      return sizeof(short);
    case type_int:
      return sizeof(int);
    case type_long:
      return sizeof(long);
    case type_unsigned_char:
      return sizeof(unsigned char);
    case type_unsigned_short:
      return sizeof(unsigned short);
    case type_unsigned_int:
      return sizeof(unsigned int);
    case type_unsigned_long:
      return sizeof(unsigned long);
    case type_bool:
      return sizeof(bool);
    case type_float:
      return sizeof(float);
    case type_double:
      return sizeof(double);
    case type_text:
      return 64;
    default:
      throw std::logic_error("unknown type");
    }
}

}
