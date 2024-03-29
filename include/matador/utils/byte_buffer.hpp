#ifndef BYTE_BUFFER_HPP
#define BYTE_BUFFER_HPP

#include "matador/utils/export.hpp"

#include <array>
#include <list>

namespace matador {

/**
 * @cond MATADOR_DEV
 * @class byte_buffer
 * @brief A buffer for bytes.
 * 
 * This class provide a buffer for bytes. The
 * bytes are added in chunks. Once the current chunk
 * of bytes is full a new empty chunk is added.
 * The bytes are appended and released from the end
 * of the buffer.
 * It is used by the object_store to serialize objects.
 */
class OOS_UTILS_API byte_buffer
{
private:
  enum { BUF_SIZE = 1 << 14 };
  typedef std::array<char, BUF_SIZE> t_data_array;

public:
  /**
   * The type of the size.
   */
  typedef t_data_array::size_type size_type;

  /**
   * @brief Create an empty buffer.
   * 
   * Create an empty buffer. One empty chunk is
   * also created.
   */
  byte_buffer();
  ~byte_buffer() = default;

  /**
   * @brief Append an amount of bytes.
   * 
   * An amount of bytes is added to the buffer.
   * 
   * @param bytes The pointer to the bytes to add.
   * @param size The size of the bytes to add.
   */
  void append(const void *bytes, size_type size);

  /**
   * @brief Release a number of bytes.
   * 
   * A number of bytes is released. The released bytes
   * are removed from the buffer.
   * 
   * @param bytes The address of the memory where the bytes should go to.
   * @param size The number of bytes released from the buffer.
   */
  void release(void *bytes, size_type size);

  /**
   * Return the size of the buffer.
   */
  size_type size() const;

  /**
   * Clear the buffer.
   */
  void clear();

private:
  struct buffer_chunk
  {
    buffer_chunk() : read_cursor(0), write_cursor(0) {}
    byte_buffer::size_type available() const { return data.size() - write_cursor; }
    byte_buffer::size_type used() const { return write_cursor - read_cursor; }
    byte_buffer::size_type released() const { return read_cursor; }
    t_data_array data;
    byte_buffer::size_type read_cursor;
    byte_buffer::size_type write_cursor;
  };
  typedef std::list<buffer_chunk> t_chunk_list;
  t_chunk_list chunk_list_;
};
/// @endcond

}

#endif /* BYTE_BUFFER_HPP */
