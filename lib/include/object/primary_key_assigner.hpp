//
// Created by sascha on 6/18/15.
//

#include "object_atomizer.hpp"

#ifndef PRIMARY_KEY_ASSIGNER_HPP
#define PRIMARY_KEY_ASSIGNER_HPP

namespace oos {

class serializable;

class primary_key_assigner : public generic_object_reader<primary_key_assigner>
{
public:
  primary_key_assigner();
  virtual ~primary_key_assigner();

public:
  void assign(serializable *o, long id);

  template < typename T >
  void read_value(const char *, T &) {}

  void read_value(const char *, char *, int ) {}

private:
  long id_ = 0;
};

}
#endif /* PRIMARY_KEY_ASSIGNER_HPP */
