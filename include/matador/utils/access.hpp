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
   * @fn void serialize(S &s, T &t)
   * @brief Serializes the given object with the given serializer
   *
   * @tparam S Type of the serializer class
   * @tparam T Type of the object to be serialized
   * @param s Instance of serializer
   * @param t Object to be serialized
   */
  template<class S, class T>
  friend inline void serialize(S &, T &) {}

  /**
   * @fn void serialize(S &s, const T &t)
   * @brief Serializes the given object with the given serializer
   *
   * @tparam S Type of the serializer class
   * @tparam T Type of the object to be serialized
   * @param s Instance of serializer
   * @param t Object to be serialized
   */
  template<class S, class T>
  friend inline void serialize(S &, const T &) {}

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

  /**
   * @brief Serializes the given object with the given serializer
   *
   * @tparam S Type of the serializer class
   * @tparam T Type of the object to be serialized
   * @param s Instance of serializer
   * @param t Object to be serialized
   */
  template<class S, class T>
  static void serialize(S &s, const T &t)
  {
    const_cast<T&>(t).serialize(s);
  }
};

}

#endif //OOS_ACCESS_HPP
