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

#ifndef BLOB_HPP
#define BLOB_HPP

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

#include <cstring>

namespace oos {

class OOS_API blob
{
public:
  typedef unsigned int size_type;
  enum { CHUNK_SIZE = 1024 };

public:
  blob();
  ~blob();
  
  /**
   * @brief Assign data to blob.
   * 
   * Assign data to blob. If data is to
   * big for blob it isn't assigned and
   * false is returned. Current data is
   * cleared.
   * 
   * @tparam T The type of the data.
   * @param val The value to assign.
   * @return True if data could be assigned.
   */
  template < typename T >
  bool assign(const T &val)
  {
    delete [] data_;
    data_ = new char[sizeof(T)];
    memcpy(data_, &val, sizeof(T));
    size_ = sizeof(T);
    return false;
  }

  /**
   * @brief Append data to blob.
   * 
   * Append data to blob. If data is to
   * big for blob it isn't appended and
   * false is returned.
   * 
   * @tparam T The type of the data.
   * @param val The value to append.
   * @return True if data could be appended.
   */
  template < typename T >
  bool append(const T &val)
  {
    if (sizeof(T) > size_) {
      return false;
    }
    return false;
  }

  size_type size() const;

  size_type capacity() const;

  const char* data() const;

private:
  char *data_;
  size_type size_;
  size_type capacity_;
};

}

#endif /* BLOB_HPP */
