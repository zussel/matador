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
#include "database/primary_key_binder.hpp"

#include <memory>
#include <unordered_map>
#include <map>
#include <list>

namespace oos {

class statement;
class serializable;
class object_base_ptr;
class object_store;
class prototype_node;

/// @cond OOS_DEV

class OOS_API table
{
public:
  typedef std::list<object_proxy*> object_proxy_list_t;                 /**< List of object proxies */
  typedef std::unordered_map<long, object_proxy_list_t> object_map_t;   /**< Map of object proxy lists where key is the primary key of the object in this table */
  typedef std::map<std::string, object_map_t> t_to_many_data;           /**< Map of object proxy list maps where key is the field name of the object */

  typedef std::map<std::shared_ptr<basic_identifier>, object_proxy*> t_to_one_data;

//protected:
  table(database &db, const prototype_node &node);

public:
  ~table();

  std::string name() const;
  virtual void prepare();
  void create();
  void load(object_store &ostore);
  void insert(serializable *obj);
  void update(serializable *obj);
  void remove(serializable *obj);
  void drop();

  bool is_loaded() const;

protected:
//  const prototype_node& node() const;
//
//  virtual database& db() { return db_; }
//  virtual const database& db() const { return db_; }

private:
  friend class relation_filler;
  friend class table_reader;

  database &db_;
  const prototype_node &node_;

  statement insert_;
  statement update_;
  statement delete_;
  statement select_;

  bool prepared_;

  bool is_loaded_;
  t_to_many_data relation_data;
  t_to_one_data to_one_data;

  primary_key_binder primary_key_binder_;
};

///@endcond

}

#endif /* TABLE_HPP */
