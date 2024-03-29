#ifndef SEQUENCER_HPP
#define SEQUENCER_HPP

#include "matador/utils/export.hpp"

#include <memory>

namespace matador {

/**
 * @cond MATADOR_DEV
 * @class sequencer_impl
 * @brief Sequencer implementation interface
 *
 * This class provides the implementation
 * interface for the sequencer class.
 */
class OOS_UTILS_API sequencer_impl
{
public:
  virtual ~sequencer_impl() = default;
  
  /**
   * Initialize the sequencer implementation.
   *
   * @return The sequence number after init.
   */
  virtual unsigned long long init() = 0;

  /**
   * Resets the sequencer implementation to
   * the new given number. The number is returned
   * afterwards.
   *
   * @param id The new id of the sequencer.
   * @return The new set id.
   */
  virtual unsigned long long reset(unsigned long long id) = 0;

  /**
   * Get the next valid sequence id.
   * The current id is incremented and
   * returned.
   *
   * @return The next valid sequence id.
   */
  virtual unsigned long long next() = 0;

  /**
   * Returns the current sequence id without
   * incrementing it.
   *
   * @return The current sequence id.
   */
  virtual unsigned long long current() const = 0;

  /**
   * Updates the given id with the current id
   * of the sequencer. The current id is set to
   * the given id if the given id is greater than
   * the current id.
   *
   * @param id The id to update
   * @return The new id.
   */
  virtual unsigned long long update(unsigned long long id) = 0;
};
/// @endcond

typedef std::shared_ptr<sequencer_impl> sequencer_impl_ptr; /**< Shortcut for sequencer implementation pointer */

/**
 * @cond MATADOR_DEV
 * @class default_sequencer
 */
class OOS_UTILS_API default_sequencer : public sequencer_impl
{
public:
  default_sequencer() = default;
  ~default_sequencer() override = default;

  unsigned long long init() override;

  unsigned long long reset(unsigned long long id) override;

  unsigned long long next() override;
  unsigned long long current() const override;

  unsigned long long update(unsigned long long id) override;

private:
  unsigned long long number_ {0};
};
/// @endcond

/**
 * @class sequencer
 * @brief Interface to create and get unique
 *        sequence numbers
 *
 * This class provides an interface to create, get
 * and manage unique sequence numbers. Because it
 * is just an interface the underlying sequencer
 * implementation is used to do the real work implementing
 * the sequencers strategy.
 */
class OOS_UTILS_API sequencer
{
public:
  /**
   * @brief Creates a new sequencer.
   *
   * This constructor creates a new sequencer.
   * If no implementation serializable is given, a
   * default implementation serializable is used.
   *
   * @param impl The sequence implementation serializable
   *             implementing the sequence strategy.
   */
  explicit sequencer(sequencer_impl_ptr impl = sequencer_impl_ptr(new default_sequencer));
  ~sequencer() = default;

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
  unsigned long long init();

  /**
   * Resets the sequencers sequence
   * id to the given value.
   *
   * @param id The new sequence number.
   * @return The current sequence number.
   */
  unsigned long long reset(unsigned long long id);

  /**
   * Returns the next valid sequence number.
   *
   * @return The next sequence number.
   */
  unsigned long long next();

  /**
   * Returns the current sequence number.
   *
   * @return The current sequence number.
   */
  unsigned long long current() const;

  /**
   * Updates the sequence number
   * to the given value.
   *
   * @param id The value to update.
   * @return The new current sequence value.
   */
  unsigned long long update(unsigned long long id);

private:
  sequencer_impl_ptr impl_;
};

}

#endif /* SEQUENCER_HPP */
