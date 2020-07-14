#ifndef MATADOR_FDSET_HPP
#define MATADOR_FDSET_HPP

#ifdef _MSC_VER
#ifdef matador_net_EXPORTS
    #define OOS_NET_API __declspec(dllexport)
    #define EXPIMP_NET_TEMPLATE
  #else
    #define OOS_NET_API __declspec(dllimport)
    #define EXPIMP_NET_TEMPLATE extern
  #endif
  #pragma warning(disable: 4251)
#else
#define OOS_NET_API
#endif

#include <set>

#include <cstddef>

#include <sys/select.h>

namespace matador {

class OOS_NET_API fdset
{
public:
  fdset(const fdset&) = delete;
  fdset& operator=(const fdset&) = delete;

public:
  fdset();
  ~fdset() = default;

  // set all bits to zero
  void reset();

  bool is_set(int fd) const;
  void clear(int fd);
  void set(int fd);

  int max() const;

  size_t count() const;

  bool empty() const;

  fd_set* get();

private:
  typedef std::set<int, std::greater<int> > int_set_t;
  int_set_t max_fd_set_;

  fd_set fd_set_ = {};
};

}

#endif //MATADOR_FDSET_HPP
