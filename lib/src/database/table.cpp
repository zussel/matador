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

#include "database/table_reader.hpp"
#include "database/table.hpp"
#include "database/database_exception.hpp"
#include "database/result.hpp"
#include "database/query.hpp"
#include "database/condition.hpp"

namespace oos {

class relation_filler : public generic_object_reader<relation_filler>
{
public:
  relation_filler(database::table_ptr &tbl)
    : generic_object_reader<relation_filler>(this)
    , info_(tbl)
    , proxy_(nullptr)
  {}
  virtual ~relation_filler() {}

  void fill()
  {
    object_proxy *first = info_->node_.op_first->next;
    object_proxy *last = info_->node_.op_marker;
    while (first != last) {
      proxy_ = first;
      proxy_->obj->deserialize(*this);
      first = first->next;
    }
  }

  template < class T >
  void read_value(const char*, T&) {}

  void read_value(const char*, char*, int) {}

  void read_value(const char *id, object_container &x)
  {
    /*
     * find the relation data identified
     * by given id
     * find the object map identified by
     * objects id
     * if both are found fill object
     * container with corresponding data
     * the data will be erased from found
     * object map
     */
    table::relation_data_t::iterator i = info_->relation_data.find(id);
    if (i != info_->relation_data.end()) {
      table::object_map_t::iterator j = i->second.find(proxy_->id());
      if (j != i->second.end()) {
        while (!j->second.empty()) {
          x.append_proxy(j->second.front());
          j->second.pop_front();
        }
      }
    }
  }

private:
  database::table_ptr &info_;
  object_proxy *proxy_;
};

table::table(database &db, const prototype_node &node)
  : db_(db)
  , node_(node)
  , prepared_(false)
  , is_loaded_(false)
{}

table::~table()
{}

std::string table::name() const
{
  return node_.type;
}

void table::prepare()
{
  query q(db_);

  std::unique_ptr<object> o(node_.producer->create());
  insert_.reset(q.insert(o.get(), node_.type).prepare());
  update_.reset(q.reset().update(node_.type, o.get()).where(cond("id").equal(0)).prepare());
  delete_.reset(q.reset().remove(node_).where(cond("id").equal(0)).prepare());
  select_.reset(q.reset().select(node_).prepare());

  prepared_ = true;
}

void table::create()
{
  query q(db_);
  
  std::unique_ptr<result> res(q.create(node_).execute());
  
  // prepare CRUD statements
  prepare();
}

void table::load(object_store &ostore)
{
  if (!prepared_) {
    prepare();
  }

  table_reader reader(*this, ostore);

  std::unique_ptr<result> res(select_->execute());

  reader.read(res.get());

  /*
   * after all tables were loaded fill
   * all object containers appearing
   * in certain object types
   */
  prototype_node::field_prototype_map_t::const_iterator first = node_.relations.begin();
  prototype_node::field_prototype_map_t::const_iterator last = node_.relations.end();
  while (first != last) {
    database::table_map_t::iterator i = db().table_map_.find(first->first);
    if (i == db().table_map_.end()) {
//      throw std::out_of_range("unknown key");
    } else {
      database::table_ptr tbl = i->second;
      if (tbl->is_loaded()) {
        relation_filler filler(tbl);
        filler.fill();
      }
    }

    ++first;
  }

  is_loaded_ = true;
}

void table::insert(object *obj)
{
  insert_->bind(obj);
  std::unique_ptr<result> res(insert_->execute());
  // Todo: check insert result == 1
}

void table::update(object *obj)
{
  int pos = update_->bind(obj);
  update_->bind(pos, obj->id());
  std::unique_ptr<result> res(update_->execute());
  // Todo: check update result
}

void table::remove(object *obj)
{
  remove(obj->id());
}

void table::remove(long id)
{
  delete_->bind(0, id);
  std::unique_ptr<result> res(delete_->execute());
  // Todo: check delete result
}

void table::drop()
{
  query q(db_);

  std::unique_ptr<result> res(q.drop(node_).execute());
  // Todo: check drop result
}

bool table::is_loaded() const
{
  return is_loaded_;
}

const prototype_node& table::node() const
{
  return node_;
}

}
