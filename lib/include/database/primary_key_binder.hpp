//
// Created by sascha on 7/13/15.
//

#include "object/object_atomizer.hpp"

#ifndef PRIMARY_KEY_BINDER_HPP
#define PRIMARY_KEY_BINDER_HPP

namespace oos {

class serializable;

class primary_key_binder : public generic_object_reader<primary_key_binder>
{
public:
  primary_key_binder() : generic_object_reader<primary_key_binder>(*this) {}
  virtual ~primary_key_binder() {}

  void bind(serializable *obj, int pos);

  
};

}

#endif /* PRIMARY_KEY_BINDER_HPP */

