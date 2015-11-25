//
// Created by sascha on 11/25/15.
//

#ifndef OOS_ACCESS_HPP
#define OOS_ACCESS_HPP

namespace oos {

class access
{
public:
  template<class S, class T>
  friend inline void serialize(S &s, const T &t);

  template<class S, class T>
  friend inline void deserialize(S &s, T &t);

  template<class S, class T>
  static void serialize(S &s, const T &t)
  {
    t.serialize(s);
  }

  template<class S, class T>
  static void deserialize(S &s, T &t)
  {
    t.deserialize(s);
  }
};

}

#endif //OOS_ACCESS_HPP
