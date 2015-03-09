#include "tools/blob.hpp"

namespace oos {

blob::blob()
{}

blob::~blob()
{
}

blob::size_type blob::size() const
{
  return data_.size();
}

blob::size_type blob::capacity() const
{
  return data_.capacity();
}

const char* blob::data() const
{
  return &data_.front();
}

}
