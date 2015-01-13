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

#include <memory>

namespace oos {

/**
 * @cond OOS_DEV
 * @class sequencer_impl
 * @brief Sequencer implementation interface
 *
 * This class provides the implementation
 * interafce for the sequencer class.
 */
class OOS_API sequencer_impl
{
public:
  virtual ~sequencer_impl() {}
  
  /**
   * Initialize the sequencer implementation.
   *
   * @return The sequence number after init.
   */
  virtual long init() = 0;

  /**
   * Resets the sequencer implementation to
   * the new given number. The number is returned
   * afterwards.
   *
   * @param id The new id of the sequencer.
   * @return The new set id.
   */
  virtual long reset(long id) = 0;

  /**
   * Get the next valid sequence id.
   * The current id is incremented and
   * returned.
   *
   * @return The next valid sequence id.
   */
  virtual long next() = 0;

  /**
   * Returns the current sequence id without
   * incrementing it.
   *
   * @return The current sequence id.
   */
  virtual long current() const = 0;

  /**
   * Updates the given id with the current id
   * of the sequencer. The current id is set to
   * the given id if the given id is greater than
   * the current id.
   *
   * @param id The id to update
   * @return The new id.
   */
  virtual long update(long id) = 0;
};
/// @endcond

typedef std::tr1::shared_ptr<sequencer_impl> sequencer_impl_ptr; /**< Shortcut for sequencer implementation pointer */

/**
 * @cond OOS_DEV
 * @class default_sequencer
 */
class OOS_API default_sequencer : public sequencer_impl
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
/// @endcond

/**
 * @class sequencer
 * @brief Interface to create and get unique
 *        sequence numbers
 *
 * This class provides an interface to create, get
 * and manage unique sequence numbers. Because it
 * is just an interface the underlaying sequencer
 * implementation is used to do the real work implementing
 * the sequencers strategy.
 */
class OOS_API sequencer
{
public:
  /**
   * @brief Creates a new sequencer.
   *
   * This constructor creates a new sequencer.
   * If no implementation object is given, a
   * default implementation object is used.
   *
   * @param impl The sequence implementation object
   *             implementing the sequence strategy.
   */
  sequencer(const sequencer_impl_ptr &impl = sequencer_impl_ptr(new default_sequencer));
  ~sequencer();

  /**
   * Replace the current sequencer
   * implementation with the new one
   * and return the old implementation.
   *
   * @param impl The new sequencer implementation.
   * @return The old sequencer implementation.
   */
  sequencer_impl_ptr exchange_sequencer(const sequencer_impl_ptr &impl);

  /**
   * Initializes the sequencer
   * and return the current number.
   *
   * @return The current sequence number.
   */
  long init();

  /**
   * Resets the sequencers sequence
   * id to the given value.
   *
   * @param id The new sequence number.
   * @return The current sequence number.
   */
  long reset(long id);

  /**
   * Returns the next valid sequence number.
   *
   * @return The next sequence number.
   */
  long next();

  /**
   * Returns the current sequence number.
   *
   * @return The current sequence number.
   */
  long current() const;

  /**
   * Updates the sequence number
   * to the given value.
   *
   * @param id The value to update.
   * @return The new current sequence value.
   */
  long update(long id);

private:
  sequencer_impl_ptr impl_;
};

}

#endif /* SEQUENCER_HPP */
