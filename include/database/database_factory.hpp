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

#include "tools/singleton.hpp"
#include "tools/factory.hpp"
#include "tools/library.hpp"

#include <string>

namespace oos {

class database_impl;
class database;

class database_factory : public oos::singleton<database_factory>
{
private:
  typedef factory<std::string, database_impl> factory_t;

private:
  friend class singleton<database_factory>;

  database_factory();

public:
  virtual ~database_factory();

  database_impl* create(const std::string &name, database *db);
  bool destroy(const std::string &name, database_impl* impl);

private:
  class database_producer : public factory_t::producer_base
  {
  public:
    explicit database_producer(const std::string &name);
    virtual ~database_producer();
    virtual factory_t::value_type* create() const;
    void destroy(factory_t::value_type* val) const;

  private:
    typedef database_impl*(*create_func)(const char*);
    typedef void (*destroy_func)(database_impl*);
    
  private:
    create_func create_;
    destroy_func destroy_;

    library loader_;
  };

private:
  factory_t factory_;
};

}

#endif /* DATABASE_FACTORY_HPP */
