#ifndef PRIMARY_KEY_HPP
#define PRIMARY_KEY_HPP

namespace oos {

class primary_key_base
{

};

// Todo: Implementation only for integer and string types
template < typename T >
class primary_key : public primary_key_base
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

  value_type operator() const {
    return pk_;
  }

private:
  T pk_;
};

}

class obj
{

  oos::primary_key<long>::value_type id() { return id_; }
private:
  oos::primary_key<long> id_;
  oos::nullable<std::string> name_;
};

#endif /* PRIMARY_KEY_HPP */