#ifndef BLOB_HPP
#define BLOB_HPP

#include "matador/utils/export.hpp"

#include <cstring>
#include <vector>

namespace matador {

/// @cond MATADOR_DEV
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
