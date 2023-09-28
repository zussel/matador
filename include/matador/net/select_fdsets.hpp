#ifndef MATADOR_SELECT_FDSETS_HPP
#define MATADOR_SELECT_FDSETS_HPP

#include "matador/net/export.hpp"
#include "matador/net/fdset.hpp"

namespace matador {

/**
 * This class represents three fd sets
 * needed for the system call to select.
 */
class OOS_NET_API select_fdsets
{
public:
  /**
   * These enum values are used
   * to identify and access the three
   * different fd sets
   */
  typedef enum {
    read_type  = 0,     /**< Enum value for the read fd set */
    write_type = 1,     /**< Enum value for the write fd set */
    except_type         /**< Enum value for the exceptional fd set */
  } fdset_type;

  /**
   * Returns the highest fd value over
   * all three fd sets plus one. This is used
   * by the system call to select.
   *
   * @return Highest fd value over all sets
   */
  socket_type maxp1() const;

  /**
   * Returns the fdset identified by the given type
   *
   * @param type Requested fdset type
   * @return The requested fd set
   */
  fdset& fd_set(fdset_type type);

  /**
   * Returns the read fd set.
   *
   * @return The read fd set.
   */
  fdset& read_set();

  /**
   * Returns the read fd set.
   *
   * @return The read fd set.
   */
  const fdset& read_set() const;

  /**
   * Returns the write fd set.
   *
   * @return The write fd set.
   */
  fdset& write_set();

  /**
   * Returns the write fd set.
   *
   * @return The write fd set.
   */
  const fdset& write_set() const;

  /**
   * Returns the exceptional fd set.
   *
   * @return The exceptional fd set.
   */
  fdset& except_set();

  /**
   * Returns the exceptional fd set.
   *
   * @return The exceptional fd set.
   */
  const fdset& except_set() const;

  /**
   * Reset all bits in all three sets to zero
   */
  void reset();

  /**
   * Resets the fd set identified by the
   * given type
   *
   * @param type Type of set to reset
   */
  void reset(fdset_type type);

  /**
   * Checks if the given fd is set in the fdset
   * identified by the given type. If fd
   * is set true returned.
   *
   * @param type Requested fd set type
   * @param fd fd to check
   * @return True if fd is set in fdset
   */
  bool is_set(fdset_type type, int fd) const;

  /**
   * Clears the given fd is set in the fdset
   * identified by the given type.
   *
   * @param type Requested fd set type
   * @param fd fd to clear
   */
  void clear(fdset_type type, int fd);

  /**
   * Sets the bit for the given fd is set
   * in the fdset identified by the given type.
   *
   * @param type Requested fd set type
   * @param fd fd to set
   */
  void set(fdset_type type, int fd);

private:
  fdset fdsets_[3];
};

}
#endif //MATADOR_SELECT_FDSETS_HPP
