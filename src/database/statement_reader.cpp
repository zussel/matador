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

statement_reader::statement_reader()
{}

statement_reader::~statement_reader()
{}

void statement_reader::import(const prototype_node &node, const statement_impl_ptr &stmt)
{
  std::cout << "importing [" << node.type << "]\n";

  // init
  stmt_ = stmt;
  column_ = 0;

  // create object
  object *o(node.producer->create());
  
  o->deserialize(*this);
  
  stmt_->db().db()->ostore().insert(o);
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

void statement_reader::read(const char *id, char *x, int )
{
  if (!valid_column(id, column_)) {
    return;
  }
  int len(0);
  const char *val = stmt_->column_text(column_++, len);
  // TODO: check buffer size
#ifdef WIN32
  // TODO: fixme: use db varchar in correct way
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
  
  std::cout << "DEBUG: reading field [" << id << "] of type [" << x.type() << "]\n";
  object_proxy *oproxy = stmt_->db().db()->ostore().find_proxy(oid);

  prototype_iterator node = stmt_->db().db()->ostore().find_prototype(x.type());
    
  // check for relations
  prototype_node::string_node_pair_list_t::const_iterator first = node->relations.begin();
  prototype_node::string_node_pair_list_t::const_iterator last = node->relations.end();
  while (first != last) {
    const prototype_node::string_node_pair_t rel = *first++;
    std::cout << "DEBUG: found relation in node [" << node->type << "] for type [" << rel.second->type << "] field [" << rel.first << "]\n";
    // check if this object is part of a relation
    
  }

  if (!oproxy) {
    oproxy = stmt_->db().db()->ostore().create_proxy(oid);
    
    //oproxy->relations[rel.second].push_back(object_);
    std::cout << "DEBUG: object [" << oid << "] of type [" << node->type << "] not in store\n";

  } else {
    std::cout << "DEBUG: object [" << oid << "] of type [" << node->type << "] found in store\n";
    
    // container_inserter reader(oproxy->obj, rel);
    // reader.read(
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
//  prototype_iterator p = stmt_->db().db()->ostore().find_prototype(x.classname());
//  const prototype_node::proxy_list_t &proxy_list = p->relations[id][object_->id()];
  
  
}

bool statement_reader::valid_column(const char *id, int i) const
{
  const char *name = stmt_->column_name(i);
  return strcmp(name, id) == 0;
}

}
