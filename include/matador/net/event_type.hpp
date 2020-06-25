#ifndef MATADOR_EVENT_TYPE_HPP
#define MATADOR_EVENT_TYPE_HPP

namespace matador {

enum class event_type {
  NONE_MASK =   0,
  READ_MASK =   1 << 0,
  WRITE_MASK =  1 << 1,
  EXCEPT_MASK = 1 << 2,
  ACCEPT_MASK = 1 << 3,
  ALL_MASK = READ_MASK | WRITE_MASK | EXCEPT_MASK | ACCEPT_MASK
};

}
#endif //MATADOR_EVENT_TYPE_HPP
