#ifndef CONDITIONAL_HPP
#define CONDITIONAL_HPP

namespace matador {

/// @cond MATADOR_DEV

template<bool B, class T, class F>
struct conditional { typedef T type; };
 
template<class T, class F>
struct conditional<false, T, F> { typedef F type; };

/// @endcond MATADOR_DEV

}

#endif /* CONDITIONAL_HPP */
