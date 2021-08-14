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
  #ifdef matador_sql_EXPORTS
    #define OOS_SQL_API __declspec(dllexport)
    #define EXPIMP_SQL_TEMPLATE
  #else
    #define OOS_SQL_API __declspec(dllimport)
    #define EXPIMP_SQL_TEMPLATE extern
  #endif
#else
  #define OOS_SQL_API
#endif

#include "matador/utils/singleton.hpp"
#include "matador/utils/factory.hpp"
#include "matador/utils/library.hpp"

#include "matador/sql/connection.hpp"

#include <string>

namespace matador {

/// @cond MATADOR_DEV
class OOS_SQL_API connection_factory : public matador::singleton<connection_factory>
{
private:
  typedef factory<std::string, connection_impl> factory_t;
  
private:
  friend class singleton<connection_factory>;

  connection_factory();

public:
  ~connection_factory() override = default;

  connection_impl* create(const std::string &name);
  bool destroy(const std::string &name, connection_impl* impl);
  
private:

  class connection_producer : public factory_t::producer_base
  {
  public:
    connection_producer() = default;
    ~connection_producer() override = default;
    factory_t::value_type* create() const override;
    virtual void destroy(factory_t::value_type* val) const;
  };

  class dynamic_connection_producer : public connection_producer
  {
  public:
    explicit dynamic_connection_producer(const std::string &name);
    ~dynamic_connection_producer() override;
    factory_t::value_type* create() const override;
    void destroy(factory_t::value_type* val) const override;

  private:
    typedef connection_impl*(*create_func)();
    typedef void (*destroy_func)(connection_impl*);

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
