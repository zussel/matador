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

#ifndef DATABASE_FACTORY_HPP
#define DATABASE_FACTORY_HPP

#ifdef WIN32
  #ifdef oos_EXPORTS
    #define OOS_API __declspec(dllexport)
    #define EXPIMP_TEMPLATE
  #else
    #define OOS_API __declspec(dllimport)
    #define EXPIMP_TEMPLATE extern
  #endif
#else
  #define OOS_API
#endif

#include "tools/singleton.hpp"
#include "tools/factory.hpp"
#include "tools/library.hpp"

#include <string>

namespace oos {

class database_impl;
class database;

/// @cond OOS_DEV

class OOS_API database_producer
{
public:
  virtual ~database_producer() {}

  virtual database_impl* create(database *db, const std::string &conn) const = 0;
};

class database_factory : public oos::singleton<database_factory>
{
private:
  class database_loader
  {
  public:
    database_loader(const std::string &name);
    ~database_loader();

    database_impl* create(database *db, const std::string &conn) const;

  private:
    typedef database_producer*(*get_producer)();

  private:
    std::string name_;
    database_producer *producer_;
    library loader_;
  };

private:
  typedef std::map<std::string, database_loader*> factory_t;

private:
  friend class singleton<database_factory>;

  database_factory();

public:
  virtual ~database_factory();

  database_impl* create(const std::string &name, database *db);
  
private:
  factory_t factory_;
};
/// @endcond
}

#endif /* DATABASE_FACTORY_HPP */
