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

namespace oos {

template < class T >
class attribute_reader : public object_atomizer
{
public:
  attribute_reader(const std::string &id, const T &from)
    : id_(id)
    , from_(from)
    , success_(false)
  {}
  
  virtual ~attribute_reader() {}

  bool success() const
  {
    return success_;
  }

  virtual void read(const char *id, char &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, float &x)
   * @brief Read a float from the atomizer.
   * 
   * Read a float from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void read(const char *id, double &x)
   * @brief Read a double from the atomizer.
   * 
   * Read a double from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void read(const char *id, short &x)
   * @brief Read an short from the atomizer.
   * 
   * Read an short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void read(const char *id, int &x)
   * @brief Read an integer from the atomizer.
   * 
   * Read an integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void read(const char *id, long &x)
   * @brief Read a long from the atomizer.
   * 
   * Read a long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void read(const char *id, unsigned short &x)
   * @brief Read an unsigned short from the atomizer.
   * 
   * Read a unsigned short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void read(const char *id, unsigned int &x)
   * @brief Read an unsigned integer from the atomizer.
   * 
   * Read a unsigned integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void read(const char *id, unsigned long &x)
   * @brief Read an unsigned long from the atomizer.
   * 
   * Read a unsigned long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void read(const char *id, bool &x)
   * @brief Read a bool from the atomizer.
   * 
   * Read a bool from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void read(const char *id, char *&x)
   * @brief Read a const char pointer from the atomizer.
   * 
   * Read a const char pointer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, char* &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to, 1);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, std::string &x)
   * @brief Read a std::string from the atomizer.
   * 
   * Read a std::string from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void read(const char *id, std::string &to)
  {
    if (id_ != id) {
      return;
    }
    convert(from_, to);
    success_ = true;
  }

  /**
   * @fn virtual void read(const char *id, varchar_base &x)
   * @brief Read a varchar from the atomizer.
   * 
   * Read a varchar from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void read(const char *id, object_base_ptr &x)
   * @brief Read an object_base_ptr from the atomizer.
   * 
   * Read an object_base_ptr from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
};

template < class T >
class attribute_writer : public object_atomizer
{
public:
  attribute_writer(const std::string &id, T &to)
    : id_(id)
    , to_(to)
    , success_(false)
  {}
  
  virtual ~attribute_writer() {}

  bool success() const
  {
    return success_;
  }

  virtual void write(const char *id, char from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, float &x)
   * @brief Read a float from the atomizer.
   * 
   * Read a float from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void write(const char *id, double &x)
   * @brief Read a double from the atomizer.
   * 
   * Read a double from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void write(const char *id, short &x)
   * @brief Read an short from the atomizer.
   * 
   * Read an short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void write(const char *id, int &x)
   * @brief Read an integer from the atomizer.
   * 
   * Read an integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void write(const char *id, long &x)
   * @brief Read a long from the atomizer.
   * 
   * Read a long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void write(const char *id, unsigned short &x)
   * @brief Read an unsigned short from the atomizer.
   * 
   * Read a unsigned short from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void write(const char *id, unsigned int &x)
   * @brief Read an unsigned integer from the atomizer.
   * 
   * Read a unsigned integer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void write(const char *id, unsigned long &x)
   * @brief Read an unsigned long from the atomizer.
   * 
   * Read a unsigned long from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void write(const char *id, bool &x)
   * @brief Read a bool from the atomizer.
   * 
   * Read a bool from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void write(const char *id, char *&x)
   * @brief Read a const char pointer from the atomizer.
   * 
   * Read a const char pointer from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
   */
	virtual void write(const char *id, const char* from)
  {
    if (id_ != id) {
      return;
    }
    convert(from, to_);
    success_ = true;
  }

  /**
   * @fn virtual void write(const char *id, std::string &x)
   * @brief Read a std::string from the atomizer.
   * 
   * Read a std::string from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void write(const char *id, varchar_base &x)
   * @brief Read a varchar from the atomizer.
   * 
   * Read a varchar from the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to write to.
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
   * @fn virtual void write(const char *id, const object_base_ptr &x)
   * @brief Write an object_base_ptr to the atomizer.
   * 
   * Write an object_base_ptr to the atomizer
   * identified by a unique name.
   * 
   * @param id Unique id of the data.
   * @param x The data to read from.
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
