#ifndef ENABLE_IF_HPP
#define ENABLE_IF_HPP

namespace matador {

/// @cond MATADOR_DEV

template<bool B, class T = void>
struct enable_if {};
 
template<class T>
struct enable_if<true, T> { typedef T type; };

/// @endcond MATADOR_DEV

}

#endif /* ENABLE_IF_HPP */
