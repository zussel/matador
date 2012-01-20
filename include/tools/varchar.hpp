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

class varchar
{
public:
  typedef std::string::size_type size_type;

public:
  explicit varchar(size_type capacity);

  varchar(const varchar &x);

  varchar& operator=(const varchar &x);

  varchar& operator=(const std::string &x);

  varchar& operator=(const char *x);

  ~varchar();
  
  varchar& operator+=(const varchar &x);

  varchar& operator+=(const std::string &x);

  varchar& operator+=(const char *x);

  operator std::string() const;
  
  std::string str() const;

  size_type size() const;

  size_type capacity() const;

  friend std::ostream& operator<<(std::ostream &out, const varchar &val);

protected:
  void ok(const std::string &x);

protected:
  size_type capacity_;
  std::string data_;
};

template < unsigned int C >
class tvarchar : public varchar
{
public:
  tvarchar()
    : varchar(C)
  {}

  tvarchar(const tvarchar &x)
    : varchar(C)
  {
    data_ = x.data_;
  }

  tvarchar& operator=(const tvarchar &x)
  {
    
    capacity_ = C;
    data_ = x.data_;
    return *this;
  }

  tvarchar& operator=(const std::string &x)
  {
    return *this;
  }

  tvarchar& operator=(const char *x)
  {
    return *this;
  }
};

}

#endif /* VARCHAR */
