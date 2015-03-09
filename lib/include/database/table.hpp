/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY {} without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TABLE_HPP
#define TABLE_HPP

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

#include "object/object_store.hpp"
#include "object/object_container.hpp"
#include "object/prototype_node.hpp"

#include "database/statement.hpp"
#include "database/database.hpp"

#include <memory>
#include <unordered_map>
#include <map>
#include <list>

namespace oos {

class statement;
class object;
class object_container;
class object_base_ptr;
class object_store;
struct prototype_node;

/// @cond OOS_DEV

class OOS_API table
{
public:
  typedef std::list<object_proxy*> object_proxy_list_t;
  typedef std::unordered_map<long, object_proxy_list_t> object_map_t;
  typedef std::map<std::string, object_map_t> relation_data_t;

//protected:
  table(database &db, const prototype_node &node);

public:
  ~table();

  std::string name() const;
  virtual void prepare();
  void create();
  void load(object_store &ostore);
  void insert(object *obj);
  void update(object *obj);
  void remove(object *obj);
  void remove(long id);
  void drop();

  bool is_loaded() const;

protected:
  const prototype_node& node() const;

  virtual database& db() { return db_; }
  virtual const database& db() const { return db_; }

private:
  friend class relation_filler;
  friend class table_reader;

  database &db_;
  const prototype_node &node_;

  typedef std::unique_ptr<statement> statement_ptr;
  statement_ptr insert_;
  statement_ptr update_;
  statement_ptr delete_;
  statement_ptr select_;

  bool prepared_;

  bool is_loaded_;
  relation_data_t relation_data;
};

///@endcond

}

#endif /* TABLE_HPP */
