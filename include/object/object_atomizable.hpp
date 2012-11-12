/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OBJECT_ATOMIZABLE_HPP
#define OBJECT_ATOMIZABLE_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
  #define OOS_API
#endif

namespace oos {

class object_writer;
class object_reader;

/**
 * @class object_atomizable
 * @brief Base class for all serializable objects
 *
 * This class provides an interface to serialize
 * and deserialize an object. It is used with
 * an instance of a concrete object_atomizer class.
 */
class OOS_API object_atomizable
{
public:
  virtual ~object_atomizable() {}

  /**
   * @fn virtual void deserialize(object_reader &deserializer)
   * Deserializes an object from the given
   * object_atomizer object.
   *
   * @param deserializer The object_atomizer to deserialize from.
   */
  virtual void deserialize(object_reader &) = 0;

  /**
   * @fn virtual void serialize(object_writer &serializer) const
   * Serializes an object to the given
   * object_atomizer object.
   *
   * @param serializer The object_atomizer to serialize to.
   */
  virtual void serialize(object_writer &) const = 0;
};

}
#endif /* OBJECT_ATOMIZABLE_HPP */
