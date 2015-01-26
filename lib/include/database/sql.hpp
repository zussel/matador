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

#include "database/types.hpp"

#include <string>
#include <map>
#include <list>
#include <vector>
#include <stdexcept>

#include <memory>

namespace oos {

/// @cond OOS_DEV

class token;
class condition;

class OOS_API sql
{
public:
  struct field
  {
    field(const char *n, data_type_t t, unsigned int i, bool h)
      : name(n), type(t), index(i), is_host(h)
    {}
    std::string name;
    data_type_t type;
    unsigned int index;
    bool is_host;
  };

  typedef std::shared_ptr<field> field_ptr;

  struct token {
    virtual ~token() {}
    
    virtual std::string get(bool) const = 0;
  };

public:
  typedef std::vector<field_ptr> field_vector_t;
  typedef field_vector_t::size_type size_type;
  typedef field_vector_t::iterator iterator;
  typedef field_vector_t::const_iterator const_iterator;

  typedef std::map<std::string, field_ptr> field_map_t;
  
  typedef std::list<token*> token_list_t;
  
public:
  ~sql();
  
  void append(const std::string &str);
  void append(const char *id, data_type_t type);
  void append(const char *id, data_type_t type, const std::string &val);
  void append(const condition &c);

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

private:
  std::string generate(bool prepared) const;

private:
  field_vector_t host_field_vector_;
  field_map_t host_field_map_;
  field_vector_t result_field_vector_;
  field_map_t result_field_map_;
  
  token_list_t token_list_;
};

/// @endcond

}

#endif /* SQL_HPP */
