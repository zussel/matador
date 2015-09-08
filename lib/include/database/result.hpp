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

#ifndef RESULT_HPP
#define RESULT_HPP

#ifdef _MSC_VER
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

#include "object/object_atomizer.hpp"

#include <memory>
#include <object/prototype_node.hpp>

namespace oos {

class row;
class statement;
class serializable;

/// @cond OOS_DEV

class OOS_API result : public object_reader
{
private:
  result(const result&) = delete;
  result& operator=(const result&) = delete;

public:
  typedef unsigned long size_type;

protected:
  result();

public:
  virtual ~result();
  
  template < class T >
  void get(unsigned long i, T &val)
  {
    result_index = transform_index(i);
    read("", val);
  }
  void get(serializable *o);
  
  virtual const char* column(size_type c) const = 0;
  virtual bool fetch() = 0;

  /*
   * Create new object and fill all builtin datatypes. If all
   * rows are processed nullptr is returned.
   *
   * @param node The corresponding prototype_node
   * @return The filled and created serializable or nullptr
   */
  serializable* fetch(const oos::prototype_node *node);

  /**
   * Fetch next line from database and
   * deserialized the given serializable.
   *
   * @param o Object to be deserialized
   * @return True if serializable was successfully deserialized
   */
  virtual bool fetch(serializable *) { return false; }

  virtual size_type affected_rows() const = 0;
  virtual size_type result_rows() const = 0;
  virtual size_type fields() const = 0;
  
  virtual int transform_index(int index) const = 0;

protected:
  virtual void read(const char *, char &) {}
  virtual void read(const char *, short &) {}
  virtual void read(const char *, int &) {}
  virtual void read(const char *, long &) {}
  virtual void read(const char *, unsigned char &) {}
  virtual void read(const char *, unsigned short &) {}
  virtual void read(const char *, unsigned int &) {}
  virtual void read(const char *, unsigned long &) {}
  virtual void read(const char *, bool &) {}
  virtual void read(const char *, float &) {}
  virtual void read(const char *, double &) {}
  virtual void read(const char *, char *, int ) {}
  virtual void read(const char *, varchar_base &) {}
  virtual void read(const char *, std::string &) {}
  virtual void read(const char *, oos::date &) {}
  virtual void read(const char *, oos::time &) {}
  virtual void read(const char *id, object_base_ptr &x);
  virtual void read(const char *id, object_container &x);
  virtual void read(const char *id, primary_key_base &x);

  const prototype_node* node() const;

protected:
  int result_index;

private:
  const prototype_node *node_ = nullptr;
};

typedef std::shared_ptr<result> result_ptr;

/// @endcond

}

#endif /* RESULT_HPP */
