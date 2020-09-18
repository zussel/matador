#ifndef MATADOR_EVENT_TYPE_HPP
#define MATADOR_EVENT_TYPE_HPP

namespace matador {

enum class event_type {
  NONE_MASK =   0,
  READ_MASK =   1 << 0,
  WRITE_MASK =  1 << 1,
  EXCEPT_MASK = 1 << 2,
  ACCEPT_MASK = READ_MASK,
  READ_WRITE_MASK = READ_MASK | WRITE_MASK,
  ALL_MASK = READ_MASK | WRITE_MASK | EXCEPT_MASK | ACCEPT_MASK
};

inline event_type operator|(event_type a, event_type b)
{
  return static_cast<event_type>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline event_type operator&(event_type a, event_type b)
{
  return static_cast<event_type>(static_cast<unsigned int>(a) & static_cast<int>(b));
}

inline bool is_event_type_set(event_type source, event_type needle)
{
  return static_cast<int>(source & needle) > 0;
}

}
#endif //MATADOR_EVENT_TYPE_HPP
