#ifndef OOS_ACCESS_HPP
#define OOS_ACCESS_HPP

namespace matador {

template < class T, template < class ... > class C >
class has_many;

template < class T >
class object_ptr;

enum cascade_type;

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

template < typename T, typename Operation >
void primary_key(Operation &op, const char* name, T& value)
{
  op.on_primary_key(name, value);
}

template< typename Operation >
void revision(Operation &op, const char* name, unsigned long rev)
{
  op.on_revision(name, rev);
}

template < typename Operation  >
void attribute(Operation &op, const char* name, char *data, size_t len)
{
  op.on_attribute(name, data, len);
}

template < typename Operation  >
void attribute(Operation &op, const char* name, std::string &data, size_t len)
{
  op.on_attribute(name, data, len);
}

template < typename T, typename Operation  >
void attribute(Operation &op, const char* name, T& pk)
{
  op.on_attribute(name, pk);
}

template < typename T, typename Operation  >
void has_one(Operation &op, const char* name, matador::object_ptr<T> &collection, matador::cascade_type cascade)
{
  op.on_has_one(name, collection, cascade);
}

template< typename T, typename Operation  >
void belongs_to(Operation &op, const char* name, matador::object_ptr<T> &object, matador::cascade_type cascade)
{
  op.on_belongs_to(name, object, cascade);
}

//template< typename T, typename Operation  >
//void has_many(Operation &op, const char* name, matador::has_many<T> &object, matador::cascade_type cascade)
//{
//  op.on_has_one(name, object, cascade);
//}

template< typename T, template < class ... > class C, typename Operation  >
void has_many(Operation &op, const char* name, matador::has_many<T, C> &container, const char *left_column, const char *right_column, matador::cascade_type cascade)
{
  op.on_has_many(name, container, left_column, right_column, cascade);
}

}

#endif //OOS_ACCESS_HPP
