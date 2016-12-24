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

#ifdef _MSC_VER
  #ifdef oos_utils_EXPORTS
    #define OOS_UTILS_API __declspec(dllexport)
    #define EXPIMP_UTILS_TEMPLATE
  #else
    #define OOS_UTILS_API __declspec(dllimport)
    #define EXPIMP_UTILS_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_UTILS_API
#endif

#include <cstring>
#include <vector>

namespace oos {

/// @cond OOS_DEV
class OOS_UTILS_API blob
{
public:
  typedef std::size_t size_type;

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
    data_.clear();
    append(val);
    return true;
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
    append(&val, sizeof(T));
    return true;
  }

  void append(const void *data, std::size_t len);

  size_type size() const;

  const unsigned char* data() const;

private:
  std::vector<unsigned char> data_;
};
/// @endcond
}

#endif /* BLOB_HPP */
