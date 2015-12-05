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

  template < class V >
  void deserialize(V &x)
  {
    oos::access::deserialize(*this, x);
  }
  void deserialize(const char*, T &x);
  template < class V >
  void deserialize(const char *id, identifier<V> &x)
  {
    reading_pk_ = true;
    // Todo; check correctness
    V val;
    deserialize(id, val);
    x.value(val);
    reading_pk_ = false;
  }

  template < class V >
  void deserialize(const char*, V &) {}
  void deserialize(const char*, char *, size_t) {}

private:
  T value_;
  bool reading_pk_ = false;
};

template < class T >
void primary_key_reader<T>::deserialize(const char*, T &x) {
  if (reading_pk_) {
    x = value_;
  }
}

//template < class T, class V >
//void primary_key_reader<T>::deserialize(const char *id, identifier<V> &x)
//{
//  reading_pk_ = true;
//  // Todo; check correctness
//  V val;
//  deserialize(id, val);
//  x.value(val);
//  reading_pk_ = false;
//}

/// @endcond

}
#endif /* PRIMARY_KEY_READER_HPP */
