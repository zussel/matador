#include "matador/object/abstract_has_many.hpp"

namespace matador {

void abstract_has_many::append(object_proxy *proxy)
{
  if (append_func_) {
    append_func_(proxy);
  }
}

void abstract_has_many::remove(object_proxy *proxy)
{
  if (remove_func_) {
    remove_func_(proxy);
  }
}

}