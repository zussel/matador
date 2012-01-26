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

#ifndef VARCHAR_HPP
#define VARCHAR_HPP

#ifdef WIN32
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

#include <string>
#include <stdexcept>

namespace oos {

class OOS_API varchar_base
{
public:
  typedef std::string::size_type size_type;

public:
  explicit varchar_base(size_type capacity);

  varchar_base(const varchar_base &x);

  varchar_base& operator=(const varchar_base &x);

  varchar_base& operator=(const std::string &x);

  varchar_base& operator=(const char *x);

  ~varchar_base();
  
  bool operator==(const varchar_base &x) const;

  bool operator!=(const varchar_base &x) const;

  varchar_base& operator+=(const varchar_base &x);

  varchar_base& operator+=(const std::string &x);

  varchar_base& operator+=(const char *x);

  void assign(const char *s, size_t n);

  std::string str() const;

  size_type size() const;

  size_type capacity() const;

  friend OOS_API std::ostream& operator<<(std::ostream &out, const varchar_base &val);

protected:
  void ok(const std::string &x);

protected:
  size_type capacity_;
  std::string data_;
};

template < unsigned int C >
class varchar : public varchar_base
{
public:
  varchar()
    : varchar_base(C)
  {}

  varchar(const varchar &x)
    : varchar_base(C)
  {
    data_ = x.data_;
  }

  varchar(const std::string &x)
    : varchar_base(C)
  {
    data_ = x;
  }

  varchar& operator=(const varchar &x)
  {
    
    capacity_ = C;
    data_ = x.data_;
    return *this;
  }

  varchar& operator=(const std::string &x)
  {
    data_ = x;
    return *this;
  }

  varchar& operator=(const char *x)
  {
    varchar_base::operator=(x);
    return *this;
  }
};

}

#endif /* VARCHAR */
