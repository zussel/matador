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

#ifndef BYTE_BUFFER_HPP
#define BYTE_BUFFER_HPP

#include <tr1/array>
#include <list>

class byte_buffer
{
public:
  enum { BUF_SIZE = 1 << 14 };
  typedef std::tr1::array<char, BUF_SIZE> t_data_array;
  typedef t_data_array::size_type size_type;

  byte_buffer();
  ~byte_buffer();

  void append(const void *bytes, size_type size);
  void release(void *bytes, size_type size);

  size_type size() const;

  void clear();

private:
  struct buffer_chunk
  {
    buffer_chunk() : read_cursor(0), write_cursor(0) {}
    byte_buffer::size_type available() const { return data.size() - write_cursor; }
    byte_buffer::size_type used() const { return write_cursor - read_cursor; }
    t_data_array data;
    byte_buffer::size_type read_cursor;
    byte_buffer::size_type write_cursor;
  };
  typedef std::list<buffer_chunk> t_chunk_list;
  t_chunk_list chunk_list_;
};

#endif /* BYTE_BUFFER_HPP */
