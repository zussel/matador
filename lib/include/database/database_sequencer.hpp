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

#include "object/serializable.hpp"

#include "tools/varchar.hpp"

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
class statement;

/// @cond OOS_DEV

class OOS_API database_sequencer : public sequencer_impl, public serializable
{
public:
  database_sequencer(database &db);
  virtual ~database_sequencer();

public:
  virtual void deserialize(object_reader &r);
  virtual void serialize(object_writer &w) const;

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
  unsigned long backup_;
  unsigned long sequence_;
  oos::varchar<64> name_;
  std::unique_ptr<statement> update_;
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
