//
// Created by sascha on 6/22/15.
//

#ifndef PRIMARY_KEY_READER_HPP
#define PRIMARY_KEY_READER_HPP

#include "object/object_atomizer.hpp"
#include "object/primary_key.hpp"

namespace oos {

template < class T >
class primary_key_reader : public generic_object_reader<primary_key_reader<T> >
{

public:
  explicit primary_key_reader(const T &value)
    : generic_object_reader<primary_key_reader<T> >(this)
    , value_(value)
  {
    value_ = value;
  }
  virtual ~primary_key_reader() { }

public:

  void read_value(const char*, T &x);
  void read_value(const char*, primary_key_base &x);

  template < class V >
  void read_value(const char*, V &) {}
  void read_value(const char*, char *, int) {}

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
void primary_key_reader<T>::read_value(const char*, primary_key_base &x)
{
  reading_pk_ = true;
  x.deserialize("", *this);
  reading_pk_ = false;
}

}
#endif /* PRIMARY_KEY_READER_HPP */
