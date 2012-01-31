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

#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

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

#ifdef WIN32
#include <memory>
#else
#include <tr1/memory>
#endif

namespace oos {

class OOS_API sequencer_impl
{
public:
  virtual ~sequencer_impl() {}
  
  virtual long init() = 0;

  virtual long reset(long id) = 0;

  virtual long next() = 0;
  virtual long current() const = 0;

  virtual long update(long id) = 0;
};

typedef std::tr1::shared_ptr<sequencer_impl> sequencer_impl_ptr;

class default_sequencer : public sequencer_impl
{
public:
  default_sequencer();
  virtual ~default_sequencer();

  virtual long init();

  virtual long reset(long id);

  virtual long next();
  virtual long current() const;

  virtual long update(long id);

private:
  long number_;
};

class sequencer
{
public:
  sequencer(const sequencer_impl_ptr &impl = sequencer_impl_ptr(new default_sequencer));
  ~sequencer();

  void exchange_sequencer(const sequencer_impl_ptr &impl);

  long init();

  long reset(long id);

  long next();
  long current() const;

  long update(long id);

private:
  sequencer_impl_ptr impl_;
};

}

#endif /* SEQUENCER_HPP */
