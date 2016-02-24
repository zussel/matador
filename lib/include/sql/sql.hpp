/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SQL_HPP
#define SQL_HPP

#ifdef _MSC_VER
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

#include "sql/types.hpp"
#include "sql/token.hpp"

#include <string>
#include <map>
#include <list>
#include <vector>
#include <stdexcept>
#include <memory>

namespace oos {

/// @cond OOS_DEV

class token;

class OOS_API sql
{
public:

public:
  typedef std::vector<detail::field_ptr> field_vector_t;
  typedef field_vector_t::size_type size_type;
  typedef field_vector_t::iterator iterator;
  typedef field_vector_t::const_iterator const_iterator;

  typedef std::map<std::string, detail::field_ptr> field_map_t;
  
  typedef std::list<token*> token_list_t;
  
public:
  ~sql();
  
  void append(const std::string &str);
  void append(const char *id, data_type_t type);
  void append(const char *id, data_type_t type, const std::string &val);
  template < class COND >
  void append(const COND &c);

  void append(token *tok);

  std::string prepare() const;
  std::string direct() const;

  void reset();

  iterator result_begin();
  iterator result_end();
  const_iterator result_begin() const;
  const_iterator result_end() const;
  size_type result_size() const;

  iterator host_begin();
  iterator host_end();
  const_iterator host_begin() const;
  const_iterator host_end() const;
  size_type host_size() const;

  static unsigned int type_size(data_type_t type);
  template < class T >
  static unsigned int data_type()
  {
    return type_traits<T>::data_type();
  }

  std::string str(bool prepared) const;

private:
  std::string generate(bool prepared) const;

private:
  field_vector_t host_field_vector_;
  field_map_t host_field_map_;
  field_vector_t result_field_vector_;
  field_map_t result_field_map_;
  
  token_list_t token_list_;
};

template < class COND >
void sql::append(const COND &)
{
//  condition_serializer cs();
//  c.serial
  // TODO: fix missing data type

//  c.accept(*this);
//  detail::field_ptr f(new detail::field(c.column().c_str(), data_type_t::type_blob , host_field_vector_.size(), true));
//
//  token_list_.push_back(new condition_token<COND>(c));
//  host_field_map_.insert(std::make_pair(c.column(), f));
//  host_field_vector_.push_back(f);
}

/// @endcond

}

#endif /* SQL_HPP */
