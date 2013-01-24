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

#include "database/statement_reader.hpp"
#include "database/statement.hpp"
#include "database/database.hpp"
#include "database/session.hpp"

#include "tools/varchar.hpp"

#include "object/object_ptr.hpp"
#include "object/object.hpp"
#include "object/object_store.hpp"
#include "object/object_container.hpp"
#include "object/prototype_node.hpp"

#include <cstring>

namespace oos {

class relation_filler : public generic_object_reader<relation_filler>
{
public:
  relation_filler(database::table_info_t &info)
    : generic_object_reader<relation_filler>(this)
    , info_(info)
    , object_(0)
  {}
  virtual ~relation_filler() {}
  
  void fill()
  {
    object_proxy *first = info_.node->op_first->next;
    object_proxy *last = info_.node->op_marker;
    while (first != last) {
      object_ = first->obj;
      object_->deserialize(*this);
      first = first->next;
    }
  }

  template < class T >
  void read_value(const char*, T&) {}
  
  void read_value(const char*, char*, int) {}
  
  void read_value(const char *id, object_container &x)
  {
//    std::cout << "DEBUG: fill container [" << id << "]\n";
    database::relation_data_t::iterator i = info_.relation_data.find(id);
    if (i != info_.relation_data.end()) {
      database::object_map_t::iterator j = i->second.find(object_->id());
      if (j != i->second.end()) {
//        std::cout << "DEBUG: found item list [" << x.classname() << "] with [" << j->second.size() << "] elements\n";
        while (!j->second.empty()) {
          x.append_proxy(j->second.front()->proxy_);
          j->second.pop_front();
        }
      }
    }
  }

private:
  database::table_info_t &info_;
  object *object_;
};

statement_reader::statement_reader(const prototype_node &node, const statement_ptr &stmt, database::table_info_t &info)
  : object_(0)
  , stmt_(stmt)
  , node_(node)
  , info_(info)
  , column_(0)
{}

statement_reader::~statement_reader()
{}

void statement_reader::import()
{
//  std::cout << "importing [" << node_.type << " (type: " << node_.producer->classname() << ")]\n";

  while (stmt_->step()) {
    column_ = 0;

    // create object
    object_ = node_.producer->create();
    
    object_->deserialize(*this);
  
    stmt_->db().db()->ostore().insert(object_);
  }
  
  prototype_node::field_prototype_node_map_t::const_iterator first = node_.relations.begin();
  prototype_node::field_prototype_node_map_t::const_iterator last = node_.relations.end();
  while (first != last) {
//    std::cout << "DEBUG: checking for relation node [" << first->first << "] ...";
//    database::table_info_t &info = stmt_->db().table_info_map_.at(first->first);
    database::table_info_map_t::iterator i = stmt_->db().table_info_map_.find(first->first);
    if (i == stmt_->db().table_info_map_.end()) {
      throw std::out_of_range("unknown key");
    } else {
      database::table_info_t &info = i->second;
      if (info.is_loaded) {
  //      std::cout << " loaded\n";
        relation_filler filler(info);
        filler.fill();
      } else {
  //      std::cout << " not loaded\n";
      }
    }

    ++first;
  }
}

void statement_reader::read(const char *id, char &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  // get column with name "id" from row,
  // convert value to char and assign value to x
  x = (char)stmt_->column_int(column_++);
}

void statement_reader::read(const char *id, float &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = (float)stmt_->column_double(column_++);
}

void statement_reader::read(const char *id, double &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_double(column_++);
}

void statement_reader::read(const char *id, short &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_reader::read(const char *id, int &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_reader::read(const char *id, long &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_reader::read(const char *id, unsigned char &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_reader::read(const char *id, unsigned short &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_reader::read(const char *id, unsigned int &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_reader::read(const char *id, unsigned long &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++);
}

void statement_reader::read(const char *id, bool &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  x = stmt_->column_int(column_++) != 0;
}

#ifdef WIN32
void statement_reader::read(const char *id, char *x, int s)
#else
void statement_reader::read(const char *id, char *x, int)
#endif
{
  if (!valid_column(id, column_)) {
    return;
  }
  int len(0);
  const char *val = stmt_->column_text(column_++, len);
  // TODO: check buffer size
#ifdef WIN32
//  strncpy_s(x, s, tmp, len);
  strncpy_s(x, s, val, len+1);
#else
  strncpy(x, val, len);
#endif
  x[len] = '\0';
}

void statement_reader::read(const char *id, std::string &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  int len(0);
  x = stmt_->column_text(column_++, len);
}

void statement_reader::read(const char *id, varchar_base &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  int len(0);
  x = stmt_->column_text(column_++, len);
}

void statement_reader::read(const char *id, object_base_ptr &x)
{
  if (!valid_column(id, column_)) {
    return;
  }
  long oid = 0;
  oid = stmt_->column_int(column_++);
  
  if (oid == 0) {
    return;
  }
  
//  std::cout << "DEBUG: reading field [" << id << "] of type [" << x.type() << "]\n";
  object_proxy *oproxy = stmt_->db().db()->ostore().find_proxy(oid);

  if (!oproxy) {
    oproxy = stmt_->db().db()->ostore().create_proxy(oid);
  }

  prototype_iterator node = stmt_->db().db()->ostore().find_prototype(x.type());
  
//  std::cout << "DEBUG: found prototype node [" << node->type << "]\n";

  /*
   * add the child object to the object proxy
   * of the parent container
   */
  database::table_info_map_t::iterator j = stmt_->db().table_info_map_.find(node->type);
  prototype_node::field_prototype_node_map_t::const_iterator i = node_.relations.find(node->type);
  if (i != node_.relations.end()) {
//    std::cout << "DEBUG: found relation node [" << i->second.first->type << "] for field [" << i->second.second << "]\n";
    j->second.relation_data[i->second.second][oid].push_back(object_);
//    std::cout << "DEBUG: store relation data in node [" << i->second.first->type << "]->[" << i->second.second << "][" << oid << "].push_back[" << *object_ << "]\n";
  }
  
  x.reset(oproxy->obj);
  
  /*
   * check if proxy is parent of a one to many
   * relationchip. If true and the object is already
   * loaded add this object to objects container
   * otherwise add it to the map inside of the
   * object proxy.
   */
  
}

void statement_reader::read(const char *id, object_container &x)
{
  /*
   * find prototype node and check if there
   * are proxies to insert for this container
   */
  prototype_iterator p = stmt_->db().db()->ostore().find_prototype(x.classname());
  if (p != stmt_->db().db()->ostore().end()) {
//    std::cout << "DEBUG: found container of type [" << p->type << "(" << x.classname() << ")] for prototype:field [" << node_.type << ":" << id << "]\n";
    if (stmt_->db().is_loaded(p->type)) {
//      std::cout << "DEBUG: " << x.classname() << " loaded; fill in field [" << id << "] of container [" << object_->id() << "]\n";
      database::relation_data_t::iterator i = info_.relation_data.find(id);
      if (i != info_.relation_data.end()) {
        database::object_map_t::iterator j = i->second.find(object_->id());
        if (j != i->second.end()) {
//          std::cout << "DEBUG: found item list [" << x.classname() << "] with [" << j->second.size() << "] elements\n";
          while (!j->second.empty()) {
            x.append_proxy(j->second.front()->proxy_);
            j->second.pop_front();
          }
        }
      }
    } else {
//      std::cout << "DEBUG: " << x.classname() << " not loaded; container will be filled after of [" << x.classname() << "] load\n";
    }
  }
}

bool statement_reader::valid_column(const char *id, int i) const
{
  const char *name = stmt_->column_name(i);
  return strcmp(name, id) == 0;
}

}
