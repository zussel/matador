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

#include <string>
#include <map>
#include <list>
#include <vector>
#include <memory>
#include <stdexcept>

namespace oos {

/// @cond OOS_DEV

template < class T >
struct type_traits;

class token;
class condition;

class sql
{
public:
  enum data_type_t {
    type_char = 0,
    type_short,
    type_int,
    type_long,
    type_unsigned_char,
    type_unsigned_short,
    type_unsigned_int,
    type_unsigned_long,
    type_float,
    type_double,
    type_bool,
    type_char_pointer,
    type_varchar,
    type_text,
    type_blob
  };

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

template <> struct type_traits<char>
{
  inline static sql::data_type_t data_type() { return sql::type_char; }
  inline static unsigned long type_size() { return sizeof(char); }
};

template <> struct type_traits<short>
{
  inline static sql::data_type_t data_type() { return sql::type_short; }
  inline static unsigned long type_size() { return sizeof(short); }
};

template <> struct type_traits<int>
{
  inline static sql::data_type_t data_type() { return sql::type_int; }
  inline static unsigned long type_size() { return sizeof(int); }
};

template <> struct type_traits<long>
{
  inline static sql::data_type_t data_type() { return sql::type_long; }
  inline static unsigned long type_size() { return sizeof(long); }
};

template <> struct type_traits<unsigned char>
{
  inline static sql::data_type_t data_type() { return sql::type_unsigned_char; }
  inline static unsigned long type_size() { return sizeof(unsigned char); }
};

template <> struct type_traits<unsigned short>
{
  inline static sql::data_type_t data_type() { return sql::type_unsigned_short; }
  inline static unsigned long type_size() { return sizeof(unsigned short); }
};

template <> struct type_traits<unsigned int>
{
  inline static sql::data_type_t data_type() { return sql::type_unsigned_int; }
  inline static unsigned long type_size() { return sizeof(unsigned int); }
};

template <> struct type_traits<unsigned long>
{
  inline static sql::data_type_t data_type() { return sql::type_unsigned_long; }
  inline static unsigned long type_size() { return sizeof(unsigned long); }
};

template <> struct type_traits<bool>
{
  inline static sql::data_type_t data_type() { return sql::type_bool; }
  inline static unsigned long type_size() { return sizeof(bool); }
};

template <> struct type_traits<float>
{
  inline static sql::data_type_t data_type() { return sql::type_float; }
  inline static unsigned long type_size() { return sizeof(float); }
};

template <> struct type_traits<double>
{
  inline static sql::data_type_t data_type() { return sql::type_double; }
  inline static unsigned long type_size() { return sizeof(double); }
};

/*
template <> struct type_traits<varchar_base>
{
  inline static sql::data_type_t data_type() { return sql::type_varchar; }
};
*/
template <> struct type_traits<const char*>
{
  inline static sql::data_type_t data_type() { return sql::type_varchar; }
  inline static unsigned long type_size() { return sizeof(const char*); }
};

template <> struct type_traits<std::string>
{
  inline static sql::data_type_t data_type() { return sql::type_text; }
  inline static unsigned long type_size() { return 1024; }
};

/*
template <> struct type_traits<object_base_ptr>
{
  inline static sql::data_type_t data_type() { return sql::type_long; }
};
*/

/// @endcond

}

#endif /* SQL_HPP */
