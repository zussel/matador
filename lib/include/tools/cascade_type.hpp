//
// Created by sascha on 12/16/15.
//

#ifndef OOS_CASCADE_TYPE_HPP
#define OOS_CASCADE_TYPE_HPP

namespace oos {

enum cascade_type
{
  NONE = 0,
  DELETE = 1,
  UPDATE = 2,
  INSERT = 4,
  ALL = DELETE | UPDATE | INSERT
};

}
#endif //OOS_CASCADE_TYPE_HPP
