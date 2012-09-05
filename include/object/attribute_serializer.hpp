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

#ifndef ATTRIBUTE_SERIALIZER_HPP
#define ATTRIBUTE_SERIALIZER_HPP

#include "object/object_atomizer.hpp"

#include "tools/convert.hpp"
#include "object/object_convert.hpp"

#include <type_traits>

/// @cond OOS_DEV

#ifdef WIN32
#define CPP11_TYPE_TRAITS_NS std::tr1
#else
#define CPP11_TYPE_TRAITS_NS std
#endif

/// @endcond OOS_DEV

namespace oos {

/**
 * @tparam T The type of the attribute to set.
 * @class attribute_reader
 * @brief Set an attribute value of an object.
 *
 * A given attribute value of a template type is
 * tried to set into an object. Therefor the attribute
 * with given name must be found and the value must
 * be convertible into the objects attribute.
 */
template < class T >
class attribute_reader : public object_reader
{
public:
  /**
   * @brief Creates an attribute_reader
   *
   * Creates an attribute_reader for an attribute id of type T
   * where id is the name of the attribute.
   *
   * @tparam T The type of the attribute.
   * @param id The name of the attribute.
   * @param from The attribute value to set.
   */
  attribute_reader(const std::string &id, const T &from)
    : id_(id)
    , from_(from)
    , success_(false)
  {}
  
  virtual ~attribute_reader() {}

  /**
   * @brief True if value was set.
   *
   * Returns true if the value could
   * be set successfully.
   *
   * @return True if value was set.
   */
  bool success() const
  {
    return success_;
  }

  /**
   * @fn virtual void read(const char *id, char &to)
   * @brief Read a char from the atomizer.
   * 
   * Read a char from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
  virtual void read(const char *id, char &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, float &to)
   * @brief Read a float from the atomizer.
   * 
   * Read a float from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
  virtual void read(const char *id, float &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, double &to)
   * @brief Read a double from the atomizer.
   * 
   * Read a double from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
  virtual void read(const char *id, double &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, short &to)
   * @brief Read an short from the atomizer.
   * 
   * Read an short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
  virtual void read(const char *id, short &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, int &to)
   * @brief Read an integer from the atomizer.
   * 
   * Read an integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
	virtual void read(const char *id, int &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, long &to)
   * @brief Read a long from the atomizer.
   * 
   * Read a long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
	virtual void read(const char *id, long &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, unsigned short &to)
   * @brief Read an unsigned short from the atomizer.
   * 
   * Read a unsigned short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
  virtual void read(const char *id, unsigned short &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, unsigned int &to)
   * @brief Read an unsigned integer from the atomizer.
   * 
   * Read a unsigned integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
  virtual void read(const char *id, unsigned int &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, unsigned long &to)
   * @brief Read an unsigned long from the atomizer.
   * 
   * Read a unsigned long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
  virtual void read(const char *id, unsigned long &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, bool &to)
   * @brief Read a bool from the atomizer.
   * 
   * Read a bool from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
	virtual void read(const char *id, bool &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, char *to,)
   * @brief Read a const char pointer from the atomizer.
   * 
   * Read a const char pointer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
	virtual void read(const char *id, char *to, int size)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to, size);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, std::string &to)
   * @brief Read a std::string from the atomizer.
   * 
   * Read a std::string from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
	virtual void read(const char *id, std::string &to)
  {
    if (id_ != id) {
      return;
    }
//    if (CPP11_TYPE_TRAITS_NS::is_floating_point<T>::value) {
//      convert(from_, to, 2);
//    } else {
      convert(from_, to);
//    }
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, varchar_base &to)
   * @brief Read a varchar from the atomizer.
   * 
   * Read a varchar from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
	virtual void read(const char *id, varchar_base &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, object_base_ptr &to)
   * @brief Read an object_base_ptr from the atomizer.
   * 
   * Read an object_base_ptr from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param to The data to write to.
   */
	virtual void read(const char *id, object_base_ptr &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

private:
  std::string id_;
  const T &from_;
  bool success_;

  size_t from_size;
  size_t to_size;
  int precision;
};

/**
 * @tparam T The type of the attribute to retieve.
 * @class attribute_writer
 * @brief Retrieve an attribute value from an object.
 *
 * A attribute value of a template type is
 * tried to retriev from an object. Therefor the attribute
 * of given name must be found and the value must
 * be convertible from the objects attribute.
 */
template < class T >
class attribute_writer : public object_writer
{
public:
  /**
   * @brief Creates an attribute_writer
   *
   * Creates an attribute_writer for an attribute id of type T
   * where id is the name of the attribute.
   *
   * @tparam T The type of the attribute.
   * @param id The name of the attribute.
   * @param to The attribute value to retrieve.
   */
  attribute_writer(const std::string &id, T &to)
    : id_(id)
    , to_(to)
    , success_(false)
  {}
  
  virtual ~attribute_writer() {}

  /**
   * @brief True if value could be retrieved.
   *
   * Returns true if the value could
   * be retrieved successfully.
   *
   * @return True if value could be retrieved.
   */
  bool success() const
  {
    return success_;
  }

  /**
   * @fn virtual void write(const char *id, char from)
   * @brief Read a char from the atomizer.
   * 
   * Read a char from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
  virtual void write(const char *id, char from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, float from)
   * @brief Read a float from the atomizer.
   * 
   * Read a float from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
  virtual void write(const char *id, float from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, double from)
   * @brief Read a double from the atomizer.
   * 
   * Read a double from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
  virtual void write(const char *id, double from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, short from)
   * @brief Read an short from the atomizer.
   * 
   * Read an short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
  virtual void write(const char *id, short from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, int from)
   * @brief Read an integer from the atomizer.
   * 
   * Read an integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
	virtual void write(const char *id, int from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, long from)
   * @brief Read a long from the atomizer.
   * 
   * Read a long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
	virtual void write(const char *id, long from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, unsigned short from)
   * @brief Read an unsigned short from the atomizer.
   * 
   * Read a unsigned short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
  virtual void write(const char *id, unsigned short from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, unsigned int from)
   * @brief Read an unsigned integer from the atomizer.
   * 
   * Read a unsigned integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
  virtual void write(const char *id, unsigned int from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, unsigned long from)
   * @brief Read an unsigned long from the atomizer.
   * 
   * Read a unsigned long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
  virtual void write(const char *id, unsigned long from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, bool from)
   * @brief Read a bool from the atomizer.
   * 
   * Read a bool from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
	virtual void write(const char *id, bool from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, const char *from)
   * @brief Read a const char pointer from the atomizer.
   * 
   * Read a const char pointer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
	virtual void write(const char *id, const char* from, int size)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, const std::string &from)
   * @brief Read a std::string from the atomizer.
   * 
   * Read a std::string from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
	virtual void write(const char *id, const std::string &from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, const varchar_base &from)
   * @brief Read a varchar from the atomizer.
   * 
   * Read a varchar from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to write to.
   */
	virtual void write(const char *id, const varchar_base &from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, const object_base_ptr &from)
   * @brief Write an object_base_ptr to the atomizer.
   * 
   * Write an object_base_ptr to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param from The data to read from.
   */
  virtual void write(const char *id, const object_base_ptr &from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }
private:
  std::string id_;
  T &to_;
  bool success_;
};

}

#endif /* ATTRIBUTE_SERIALIZER_HPP */
