#ifndef MATADOR_EVENT_TYPE_HPP
#define MATADOR_EVENT_TYPE_HPP

namespace matador {

/**
 * Enum representing an event processing mask
 * for the reactor io dispatcher. The mask values
 * can be bitwise combined and are used to
 * check if a handler (@see handler.hpp) is able
 * to read, write or accept.
 */
enum class event_type : unsigned {
  NONE_MASK =   0,            /**< Enum value for no event */
  READ_MASK =   1 << 0,       /**< Enum value for read mask */
  WRITE_MASK =  1 << 1,       /**< Enum value for write mask */
  EXCEPT_MASK = 1 << 2,       /**< Enum value for except mask */
  TIMEOUT_MASK = 1 << 3,      /**< Enum value for timeout mask */
  ACCEPT_MASK = READ_MASK,    /**< Enum value for accept mask */
  READ_WRITE_MASK = READ_MASK | WRITE_MASK,                     /**< Enum value for read write mask */
  ALL_MASK = READ_MASK | WRITE_MASK | EXCEPT_MASK | ACCEPT_MASK /**< Enum value for all events mask */
};

/**
 * event_type operator to concat to event types
 * with or operator
 *
 * @param a Left event_type
 * @param b Right event_type
 * @return The combined result
 */
inline event_type operator|(event_type a, event_type b)
{
  return static_cast<event_type>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

/**
 * event_type operator to concat to event types
 * with and operator
 *
 * @param a Left event_type
 * @param b Right event_type
 * @return The combined result
 */
inline event_type operator&(event_type a, event_type b)
{
  return static_cast<event_type>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

/**
 * event_type not operator
 *
 * @param a Left event_type
 * @return The combined result
 */
inline event_type operator~ (event_type a)
{
  return (event_type)~(int)a;
}

/**
 * event_type operator to concat to event types
 * with xor operator
 *
 * @param a Left event_type
 * @param b Right event_type
 * @return The combined result
 */
inline event_type operator^ (event_type a, event_type b)
{
  return (event_type)((int)a ^ (int)b);
}

/**
 * event_type operator to assign event type
 * with or operator
 *
 * @param a Left event_type
 * @param b Right event_type
 * @return The combined result
 */
inline event_type& operator|= (event_type& a, event_type b)
{
  return (event_type&)((int&)a |= (int)b);
}

/**
 * event_type operator to assign event type
 * with and operator
 *
 * @param a Left event_type
 * @param b Right event_type
 * @return The combined result
 */
inline event_type& operator&= (event_type& a, event_type b)
{
  return (event_type&)((int&)a &= (int)b);
}

/**
 * event_type operator to assign event type
 * with xor operator
 *
 * @param a Left event_type
 * @param b Right event_type
 * @return The combined result
 */
inline event_type& operator^= (event_type& a, event_type b)
{
  return (event_type&)((int&)a ^= (int)b);
}
/**
 * Checks if a specific event_type is set in
 * a given event type mask. If type is set
 * true is returned.
 *
 * @param source Event type mask to check.
 * @param needle Requested event type
 * @return True if event type is set
 */
inline bool is_event_type_set(event_type source, event_type needle)
{
  return static_cast<int>(source & needle) > 0;
}

}
#endif //MATADOR_EVENT_TYPE_HPP
