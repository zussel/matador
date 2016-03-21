//
// Created by sascha on 21.03.16.
//

#ifndef OOS_SESSION_HPP
#define OOS_SESSION_HPP

#include "object/object_ptr.hpp"

#include "orm/persistence.hpp"

namespace oos {

class session
{
public:
  explicit session(persistence &p);

  template < class T >
  object_ptr<T> insert(T *obj)
  {
    transaction tr(persistence_.store());
    tr.begin();
    object_ptr<T> optr(persistence_.store().insert(obj));
    tr.commit();
    return optr;
  }

  object_store& store();
  const object_store& store() const;

private:
  persistence &persistence_;
};

}

#endif //OOS_SESSION_HPP
