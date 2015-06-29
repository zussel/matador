//
// Created by sascha on 6/29/15.
//

#ifndef OOS_GENERIC_ACCESS_HPP
#define OOS_GENERIC_ACCESS_HPP

#include "object/attribute_serializer.hpp"

namespace oos {

/**
 * Sets a value of a member identified by
 * the given name. If the operation succeeds
 * true is returned.
 *
 * @tparam O     The object for which the value should be set.
 * @tparam T     The type of the value to set.
 * @param name   The name of the member variable.
 * @param val    The new value for the member.
 * @return       True if the operation succeeds.
 */
template < typename O, class T >
bool set(O &obj, const std::string &name, const T &val)
{
  attribute_reader<T> reader(name, val);
  obj->deserialize(reader);
  return reader.success();
}

/**
 * Gets the value of a member identified by
 * the given name. If the operation succeeds
 * true is returned.
 *
 * @tparam O     The object for which the value should be get.
 * @tparam T     The type of the value to retrieve.
 * @param name   The name of the member variable.
 * @param val    The reference where the value is assigned to.
 * @return       True if the operation succeeds.
 */
template < typename O, class T >
bool get(const O &obj, const std::string &name, T &val)
{
  attribute_writer<T> writer(name, val);
  obj->serialize(writer);
  return writer.success();
}

/**
 * Gets the value of a member identified by
 * the given name. If the operation succeeds
 * true is returned.
 *
 * @tparam O     The object for which the value should be get.
 * @tparam T        The type of the value to retrieve.
 * @param name      The name of the member variable.
 * @param val       The reference where the value is assigned to.
 * @param precision The precision of the value to get.
 * @return          True if the operation succeeds.
 */
template < typename O, class T >
bool get(const O &obj, const std::string &name, T &val, int precision)
{
  attribute_writer<T> writer(name, val, precision);
  obj->serialize(writer);
  return writer.success();
}

/**
 * Print the serializable to a given stream
 *
 * @param os The stream to write the serializable on.
 * @param o The serializable to write
 * @return The modified stream.
 */
}
#endif //OOS_GENERIC_ACCESS_HPP
