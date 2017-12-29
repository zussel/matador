//
// Created by sascha on 11/25/15.
//

#ifndef OOS_ACCESS_HPP
#define OOS_ACCESS_HPP

namespace matador {

/**
 * @class access
 * @brief Give access to the private members to a serializer
 */
class access
{
public:
  /**
   * @brief Serializes the given object with the given serializer
   *
   * @tparam S Type of the serializer class
   * @tparam T Type of the object to be serialized
   * @param s Instance of serializer
   * @param t Object to be serialized
   */
  template<class S, class T>
  friend inline void serialize(S &s, T &t);

  /**
   * @brief Serializes the given object with the given serializer
   *
   * @tparam S Type of the serializer class
   * @tparam T Type of the object to be serialized
   * @param s Instance of serializer
   * @param t Object to be serialized
   */
  template<class S, class T>
  static void serialize(S &s, T &t)
  {
    t.serialize(s);
  }
};

}

#endif //OOS_ACCESS_HPP
