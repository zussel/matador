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

namespace oos {

class database_sequencer : public default_sequencer
{
public:
  virtual ~database_sequencer() {}

protected:
  database_sequencer() {}

public:
  virtual void create() = 0;
  virtual void begin() = 0;
  virtual void commit() = 0;
  virtual void rollback() = 0;
  virtual void drop() = 0;
  virtual void destroy() = 0;
};

}

#endif /* DATABASE_SEQUENCER_HPP */
