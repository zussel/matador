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

#ifdef _MSC_VER
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

class database;
class session;

/// @cond OOS_DEV
class database_factory : public oos::singleton<database_factory>
{
private:
  typedef factory<std::string, database> factory_t;
  
private:
  friend class singleton<database_factory>;

  database_factory();

public:
  virtual ~database_factory();

  database* create(const std::string &name, session *db);
  bool destroy(const std::string &name, database* impl);
  
private:

  class database_producer : public factory_t::producer_base
  {
  public:
    explicit database_producer() : db_(nullptr) {};
    virtual ~database_producer() {};
    virtual factory_t::value_type* create() const;
    virtual factory_t::value_type* create(session *db);
    virtual void destroy(factory_t::value_type* val) const;

  protected:
    session *db_;
  };

  class dynamic_database_producer : public database_producer
  {
  public:
    explicit dynamic_database_producer(const std::string &name);
    virtual ~dynamic_database_producer();
    virtual factory_t::value_type* create() const;
    virtual void destroy(factory_t::value_type* val) const;

  private:
    typedef database*(*create_func)(session*);
    typedef void (*destroy_func)(database*);

  private:
    create_func create_;
    destroy_func destroy_;

    library loader_;
  };

private:
  factory_t factory_;
};
/// @endcond
}

#endif /* DATABASE_FACTORY_HPP */
