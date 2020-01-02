//
// Created by sascha on 02.01.20.
//

#ifndef MATADOR_MYSQL_BOOL_HPP
#define MATADOR_MYSQL_BOOL_HPP

#include <vector>

namespace matador {

namespace mysql {

typedef std::vector<bool> t_bool_vector;

template < class T >
void set_my_bool(T &mybool, t_bool_vector::reference ref_bool) {
  mybool = decltype(mybool)(&ref_bool);
}

template < class T >
void set_my_bool(T &mybool, bool &b) {
  mybool = decltype(mybool)(b);
}

}
}

#endif //MATADOR_MYSQL_BOOL_HPP
