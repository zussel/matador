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

#ifndef DATABASE_SEQUENCER_HPP
#define DATABASE_SEQUENCER_HPP

#include "tools/sequencer.hpp"
#include "tools/varchar.hpp"

#include "object/serializable.hpp"

#include "database/statement.hpp"

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

namespace oos {

class database;

/// @cond OOS_DEV

class sequence : public serializable
{
public:
  sequence();
  virtual ~sequence();

  virtual void deserialize(deserializer &r);
  virtual void serialize(serializer &w) const;


  unsigned long seq() const;
  void seq(unsigned long sequence);

  const varchar<64> &name() const;
  void name(const varchar<64> &name);

  unsigned long operator++();

private:
  unsigned long sequence_ = 0;
  oos::varchar<64> name_;
};
class OOS_API database_sequencer : public sequencer_impl
{
public:
  database_sequencer(database &db);
  virtual ~database_sequencer();

  virtual unsigned long init();
  virtual unsigned long reset(unsigned long id);
  virtual unsigned long next();
  virtual unsigned long current() const;
  virtual unsigned long update(unsigned long id);

  virtual void create();
  virtual void load();
  virtual void begin();
  virtual void commit();
  virtual void rollback();
  virtual void drop();
  virtual void destroy();
  
protected:
  long backup_sequence() const;
  void backup_sequence(long backup);

private:
  database &db_;
  sequence sequence_;
  unsigned long backup_;
  statement<sequence> update_;
};

class dummy_database_sequencer : public database_sequencer
{
public:
  dummy_database_sequencer(database &db)
    : database_sequencer(db)
    , backup_(0)
  {}
  virtual ~dummy_database_sequencer() {}

  virtual void create() {}
  virtual void load() {}
  virtual void begin() { backup_ = current(); }
  virtual void commit() {backup_ = 0; }
  virtual void rollback() { reset(backup_); }
  virtual void drop() {}
  virtual void destroy() {}

private:
  long backup_;
};

/// @endcond

}

#endif /* DATABASE_SEQUENCER_HPP */
