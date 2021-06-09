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

#include "matador/utils/byte_buffer.hpp"

#include <cstring>

namespace matador {

byte_buffer::byte_buffer()
{
  chunk_list_.push_back(buffer_chunk());
}

void byte_buffer::append(const void *bytes, byte_buffer::size_type size)
{
  const char *ptr = (const char*)bytes;
  size_type bytes_written = 0;
  while (chunk_list_.back().available() < size) {
    buffer_chunk &chunk = chunk_list_.back();
    // copy first part of bytes

#ifdef _WIN32
    memcpy_s(&chunk.data[chunk.write_cursor], chunk.available(), ptr + bytes_written, chunk.available());
#else
    memcpy(&chunk.data[chunk.write_cursor], ptr + bytes_written, chunk.available());
#endif
    //std::copy(ptr+bytes_written, ptr+bytes_written+chunk.available(), &chunk.data[chunk.write_cursor]);
    // adjust capacity size
    size -= chunk.available();
    // adjust written bytes
    bytes_written += chunk.available();
    // adjust cursor
    chunk.write_cursor += chunk.available();
    // append new buffer chunk
    chunk_list_.push_back(buffer_chunk());
  }
  if (size > 0) {
    buffer_chunk &chunk = chunk_list_.back();
    // append capacity bytes
#ifdef _WIN32
    memcpy_s(&chunk.data[chunk.write_cursor], chunk.available(), ptr + bytes_written, size);
#else
    memcpy(&chunk.data[chunk.write_cursor], ptr + bytes_written, size);
#endif
//    std::copy(ptr+bytes_written, ptr+bytes_written+size, &chunk.data[chunk.write_cursor]);
    chunk.write_cursor += size;
  }
}

void byte_buffer::release(void *bytes, byte_buffer::size_type size)
{
  char *ptr = (char*)bytes;
  size_type bytes_read = 0;
  while (!chunk_list_.empty() && chunk_list_.front().used() <= size) {
    buffer_chunk &chunk = chunk_list_.front();
    // available bytes are not enough
    // copy first part
    std::copy(chunk.data.begin()+chunk.read_cursor, chunk.data.begin()+chunk.read_cursor+chunk.used(), ptr+bytes_read);
    // adjust capacity size
    size -= chunk.used();
    // adjust bytes read
    bytes_read += chunk.used();
    // pop back
    chunk_list_.pop_front();
    // get next chunk
  }
  if (size > 0) {
    buffer_chunk &chunk = chunk_list_.front();
    // copy rest of bytes
    std::copy(chunk.data.begin()+chunk.read_cursor, chunk.data.begin()+chunk.read_cursor+size, ptr+bytes_read);
    // adjust read cursor
    chunk.read_cursor += size;
  }
  if (chunk_list_.empty()) {
    chunk_list_.push_back(buffer_chunk());
  }
}

byte_buffer::size_type byte_buffer::size() const
{
  return (chunk_list_.size() * BUF_SIZE) - chunk_list_.back().available() - chunk_list_.front().released();
  if (chunk_list_.size() == 1) {
    return BUF_SIZE - chunk_list_.front().available();
  } else {
    return (chunk_list_.size() * BUF_SIZE) - chunk_list_.back().available();
  }
}

void byte_buffer::clear()
{
  chunk_list_.clear();
  chunk_list_.push_back(buffer_chunk());
}

}
