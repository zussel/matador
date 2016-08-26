//
// Created by sascha on 8/26/16.
//

#ifndef OOS_RELATION_TABLE_LOADER_HPP
#define OOS_RELATION_TABLE_LOADER_HPP

#include "tools/access.hpp"
#include "tools/cascade_type.hpp"

#include <cstddef>
#include <iostream>

namespace oos {

namespace detail {

template < class OWNER >
class relation_table_loader
{
public:

  void load()
  {
    oos::access::serialize(*this, owner_);
  }

  template<class T>
  void serialize(T &x)
  {
    oos::access::serialize(*this, x);
  }

  template<class T>
  void serialize(const char *, T &) {}
  void serialize(const char *, char *, size_t) { }

  template < class HAS_ONE >
  void serialize(const char*, HAS_ONE&, cascade_type) { }

  template < class HAS_MANY >
  void serialize(const char *id, HAS_MANY&, const char*, const char*)
  {
    std::cout << "loading relation table for '" << id << "'\n";
  }

private:
  OWNER owner_;
};

}
}
#endif //OOS_RELATION_TABLE_LOADER_HPP
