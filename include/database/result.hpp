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

#include <iterator>

namespace oos {

class row;
class statement;

class OOS_API result_impl
{
public:
  virtual ~result_impl();

};

class result;

class result_iterator : public std::iterator<std::input_iterator_tag, row>
{
public:
  typedef result_iterator self;
  typedef row value_type;
  typedef value_type* pointer;
  typedef value_type& reference;

  result_iterator();
  explicit result_iterator(result *res);
  result_iterator(const result_iterator &x);
  result_iterator& operator=(const result_iterator &x);
  ~result_iterator();

  self& operator++();

  self operator++(int);

  pointer operator->() const;

  reference operator*() const;

  bool operator==(const self &x) const;

  bool operator!=(const self &x) const;

private:
  result *res_;
};

class OOS_API result
{
public:
  explicit result(statement *stmt);
  ~result();

  /**
   * Get next row of result. If no further
   * row is available false is returned.
   *
   * @return True if another could be fetched.
   */
  bool step();

  /**
   * Return the current row of the result set.
   * If now row is available 0 (null) is returned.
   *
   * @return The next row of the result set.
   */
  row* current() const;

  /**
   * Return the first row of the result set.
   *
   * @return The first row of the result.
   */
  result_iterator begin();

  /**
   * Return the last row of the result set.
   *
   * @return The last row of the result.
   */
  result_iterator end();

private:
  statement *stmt_;
  row *current_;
};

}

#endif /* RESULT_HPP */
