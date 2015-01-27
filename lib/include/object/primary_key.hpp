#ifndef PRIMARY_KEY_HPP
#define PRIMARY_KEY_HPP

#include "object/object_atomizer.hpp"

#include "tools/enable_if.hpp"

#include <type_traits>

namespace oos {

class primary_key_base
{
public:
  virtual ~primary_key_base() {}
  virtual void serialize(const char*, object_writer&) const = 0;
  virtual void deserialize(const char*, object_reader&) = 0;
};

template < typename T, class Enable = void >
class primary_key;

// Todo: Implementation only for integer and string types
template < typename T >
class primary_key<T, typename oos::enable_if<std::is_integral<T>::value>::type > : public primary_key_base
{
public:
  typedef T value_type;

public:
  primary_key() : pk_(0) {}
  primary_key(primary_key::value_type pk)
    : pk_(pk)
  {}

  primary_key& operator=(primary_key::value_type pk)
  {
    pk_ = pk;
    return *this;
  }

  virtual ~primary_key() {}
  virtual void serialize(const char *id, object_writer &writer) const
  {
    writer.write(id, pk_);
  }

  virtual void deserialize(const char *id, object_reader &reader)
  {
    reader.read(id, pk_);
  }

  value_type get() const
  {
    return pk_;
  }

  operator value_type() const {
    return pk_;
  }

private:
  T pk_;
};

}

/*
class obj
{

  oos::primary_key<long>::value_type id() { return id_; }
private:
  oos::primary_key<long> id_;
  oos::nullable<std::string> name_;
};
*/

#endif /* PRIMARY_KEY_HPP */
