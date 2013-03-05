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

#include "object/object_atomizer.hpp"
#include "object/object_store.hpp"
#include "object/object_container.hpp"
#include "object/prototype_node.hpp"

#include "database/statement.hpp"
#include "database/database.hpp"

#ifdef WIN32
#include <memory>
#include <unordered_map>
#else
#include <tr1/memory>
#include <tr1/unordered_map>
#endif

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

class OOS_API table : public generic_object_reader<table>
{
public:
  typedef std::list<object*> object_list_t;
  typedef std::tr1::unordered_map<long, object_list_t> object_map_t;
  typedef std::map<std::string, object_map_t> relation_data_t;

//protected:
  table(database &db, const prototype_node &node);

public:
  virtual ~table();

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

  template < class T >
  void read_value(const char *, T &) {}
  void read_value(const char *, char *, int ) {}
  void read_value(const char *, object_base_ptr &x);
  void read_value(const char *id, object_container &x);

protected:
  const prototype_node& node() const;

  virtual database& db() { return db_; }
  virtual const database& db() const { return db_; }

/*
  virtual statement* select() = 0;
  virtual statement* insert() = 0;
  virtual statement* update() = 0;
  virtual statement* remove() = 0;
*/

private:
  friend class relation_filler;

  database &db_;
  const prototype_node &node_;
  int column_;

  statement *insert_;
  statement *update_;
  statement *delete_;
  statement *select_;
  
  // temp data while loading
  object *object_;
  object_store *ostore_;
  
  bool prepared_;

  bool is_loaded_;
  relation_data_t relation_data;
};

///@endcond

}

#endif /* TABLE_HPP */
