//
// Created by sascha on 6/22/15.
//

#ifndef PRIMARY_KEY_READER_HPP
#define PRIMARY_KEY_READER_HPP

#include "object/identifier.hpp"

namespace oos {

/// @cond OOS_DEV

template < class T >
class primary_key_reader
{

public:
  explicit primary_key_reader(const T &value)
    : value_(value)
  {
    value_ = value;
  }
  ~primary_key_reader() { }

public:

  void read_value(const char*, T &x);
  void read_value(const char*, basic_identifier &x);

  template < class V >
  void read_value(const char*, V &) {}
  void read_value(const char*, char *, size_t) {}

private:
  T value_;
  bool reading_pk_ = false;
};

template < class T >
void primary_key_reader<T>::read_value(const char*, T &x) {
  if (reading_pk_) {
    x = value_;
  }
}

template < class T >
void primary_key_reader<T>::read_value(const char*, basic_identifier &x)
{
  reading_pk_ = true;
  x.deserialize("", *this);
  reading_pk_ = false;
}

/// @endcond

}
#endif /* PRIMARY_KEY_READER_HPP */
