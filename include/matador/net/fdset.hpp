#ifndef MATADOR_FDSET_HPP
#define MATADOR_FDSET_HPP

#include "matador/net/export.hpp"

#include <set>
#include <functional>
#include <cstddef>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/select.h>
#endif

namespace matador {

/**
 * This class represents a fd set
 * used by the reactor class. There it
 * is used in combination with a call
 * to select.
 */
class OOS_NET_API fdset
{
public:
  fdset(const fdset&) = delete;
  fdset& operator=(const fdset&) = delete;

  /**
   * Default constructor creates
   * an empty fd set
   */
  fdset();

  /**
   * Destroys the fd set
   */
  ~fdset() = default;

  fdset(fdset&& x) noexcept;
  fdset& operator=(fdset&& x) noexcept;

  /**
   * Reset all bits to zero
   */
  void reset();

  /**
   * Checks if the given fd is set
   * in the fd set.
   *
   * @param fd Requested fd
   * @return True if fd is set
   */
  bool is_set(int fd) const;

  /**
   * Clears the giveb fd from the set.
   *
   * @param fd fd to clear
   */
  void clear(int fd);

  /**
   * Sets the given fd in the fd set.
   *
   * @param fd fd to set
   */
  void set(int fd);

  /**
   * Returns the highest fd plus one.
   * This is needed for the call to select
   *
   * @return Highest fd plus one
   */
  int maxp1() const;

  /**
   * Returns the current number of fd in the set
   *
   * @return Number of fd in set
   */
  size_t count() const;

  /**
   * Checks if the set is empty
   *
   * @return True if the fd set is empty
   */
  bool empty() const;

  /**
   * Returns a pointer to the underlying fd_set structure
   *
   * @return Pointer to the underlying fd_set structure
   */
  fd_set* get();

private:
  typedef std::set<int, std::greater<> > int_set_t;
  int_set_t max_fd_set_;

  fd_set fd_set_ = {};
};

}

#endif //MATADOR_FDSET_HPP
