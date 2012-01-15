#include "tools/blob.hpp"

namespace oos {

blob::blob()
  : data_(new char[CHUNK_SIZE])
  , size_(0)
  , capacity_(CHUNK_SIZE)
{
}

blob::~blob()
{
  delete [] data_;
}

blob::size_type blob::size() const
{
  return size_;
}

blob::size_type blob::capacity() const
{
  return capacity_;
}

}
