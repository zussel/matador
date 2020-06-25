#ifndef MATADOR_SELECT_FDSETS_HPP
#define MATADOR_SELECT_FDSETS_HPP

#include "matador/net/fdset.hpp"

namespace matador {

class select_fdsets
{
public:
  typedef enum {
    read_type  = 0,
    write_type = 1,
    except_type
  } fdset_type;

  int max() const;

  fdset& fd_set(fdset_type type);

  fdset& read_set();
  const fdset& read_set() const;
  fdset& write_set();
  const fdset& write_set() const;
  fdset& except_set();
  const fdset& except_set() const;

  // set all bits to zero
  void reset();
  void reset(fdset_type type);

  bool is_set(fdset_type type, int fd) const;
  void clear(fdset_type type, int fd);
  void set(fdset_type type, int fd);

private:
  fdset fdsets_[3];
};

}
#endif //MATADOR_SELECT_FDSETS_HPP
