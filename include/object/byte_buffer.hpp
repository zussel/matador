#ifndef BYTE_BUFFER_HPP
#define BYTE_BUFFER_HPP

#include <tr1/array>
#include <list>

class byte_buffer
{
public:
  //enum { BUF_SIZE = 1 << 14 };
  enum { BUF_SIZE = 4 };
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
