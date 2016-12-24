#include "oos/utils/blob.hpp"

namespace oos {

blob::blob()
{}

blob::~blob()
{
}

void blob::append(const void *data, std::size_t len)
{
  data_.insert(data_.begin(), (unsigned char*)data, (unsigned char*)(data)+len);
}

blob::size_type blob::size() const
{
  return data_.size();
}

const unsigned char* blob::data() const
{
  return &data_.front();
}

}
