//
// Created by sascha on 21.03.16.
//

#include "orm/session.hpp"

namespace oos {


session::session(persistence &p)
  : persistence_(p)
{

}

object_store &session::store()
{
  return persistence_.store();
}

const object_store &session::store() const
{
  return persistence_.store();
}


}