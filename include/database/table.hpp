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

class OOS_API table
  : public generic_object_reader<table>
  , public generic_object_writer<table>
{
public:
  typedef std::list<object*> object_list_t;
  typedef std::tr1::unordered_map<long, object_list_t> object_map_t;
  typedef std::map<std::string, object_map_t> relation_data_t;

  explicit table(database &db, const prototype_node &node);
  ~table();

  std::string name() const;
  void prepare();
  void create();
  void load(object_store &ostore);
  void insert(object *obj);
  void update(object *obj);
  void remove(object *obj);
  void remove(long id);
  void drop();

  bool is_loaded() const;

  template < class T >
  void read_value(const char *, T &x)
  {
    select_->column(column_++, x);
  }

  void read_value(const char *, char *x, int s)
  {
    select_->column(column_++, x, s);
  }

  void read_value(const char *, varchar_base &x)
  {
    std::string val;
    select_->column(column_++, val);
    x = val;
  }

  void read_value(const char *, object_base_ptr &x)
  {
    long oid = 0;
//    std::cout << "DEBUG: reading field [" << id << "] (column: " << column_ << ")\n";
    select_->column(column_++, oid);
    
    
    if (oid == 0) {
      return;
    }
    
//    std::cout << "DEBUG: reading field [" << id << "] of type [" << x.type() << "]\n";
    object_proxy *oproxy = ostore_->find_proxy(oid);

    if (!oproxy) {
      oproxy = ostore_->create_proxy(oid);
    }

    prototype_iterator node = ostore_->find_prototype(x.type());
    
//    std::cout << "DEBUG: found prototype node [" << node->type << "]\n";

    /*
     * add the child object to the object proxy
     * of the parent container
     */
    database::table_map_t::iterator j = db_.table_map_.find(node->type);
    prototype_node::field_prototype_node_map_t::const_iterator i = node_.relations.find(node->type);
    if (i != node_.relations.end()) {
//      std::cout << "DEBUG: found relation node [" << i->second.first->type << "] for field [" << i->second.second << "]\n";
      j->second->relation_data[i->second.second][oid].push_back(object_);
//      std::cout << "DEBUG: store relation data in node [" << i->second.first->type << "]->[" << i->second.second << "][" << oid << "].push_back[" << *object_ << "]\n";
    }
    
    x.reset(oproxy->obj);
  }

  void read_value(const char *id, object_container &x)
  {
    /*
     * find prototype node and check if there
     * are proxies to insert for this container
     */
    prototype_iterator p = ostore_->find_prototype(x.classname());
    if (p != ostore_->end()) {
//      std::cout << "DEBUG: found container of type [" << p->type << "(" << x.classname() << ")] for prototype:field [" << node_.type << ":" << id << "]\n";
      if (db_.is_loaded(p->type)) {
//        std::cout << "DEBUG: " << x.classname() << " loaded; fill in field [" << id << "] of container [" << object_->id() << "]\n";
        database::relation_data_t::iterator i = relation_data.find(id);
        if (i != relation_data.end()) {
          database::object_map_t::iterator j = i->second.find(object_->id());
          if (j != i->second.end()) {
//            std::cout << "DEBUG: found item list [" << x.classname() << "] with [" << j->second.size() << "] elements\n";
            while (!j->second.empty()) {
              x.append_proxy(j->second.front()->proxy_);
              j->second.pop_front();
            }
          }
        }
      } else {
//        std::cout << "DEBUG: " << x.classname() << " not loaded; container will be filled after of [" << x.classname() << "] load\n";
      }
    }
  }

  template < class T >
  void write_value(const char *, const T &x)
  {
    if (inserting_) {
      insert_->bind(++column_, x);
    } else {
      update_->bind(++column_, x);
    }
  }
  void write_value(const char *, const char *x, int s)
  {
    if (inserting_) {
      insert_->bind(++column_, x, s);
    } else {
      update_->bind(++column_, x, s);
    }
  }
  void write_value(const char *, const varchar_base &x)
  {
    if (inserting_) {
      insert_->bind(++column_, x.str());
    } else {
      update_->bind(++column_, x.str());
    }
  }
  void write_value(const char *, const object_base_ptr &x)
  {
    if (inserting_) {
      if (x.ptr()) {
        insert_->bind(++column_, x.id());
      } else {
        insert_->bind_null(++column_);
      }
    } else {
      if (x.ptr()) {
        update_->bind(++column_, x.id());
      } else {
        update_->bind_null(++column_);
      }
    }
  }
  void write_value(const char *, const object_container &) {}

private:
  friend class relation_filler;

  std::string create_;
  std::string drop_;

  statement* select_;
  statement* insert_;
  statement* update_;
  statement* delete_;

  database &db_;
  const prototype_node &node_;
  int column_;

  // temp data while loading
  object *object_;
  object_store *ostore_;
  
  bool inserting_;
  bool prepared_;

  bool is_loaded_;
  relation_data_t relation_data;
};

///@endcond

}

#endif /* TABLE_HPP */
