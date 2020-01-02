//
// Created by sascha on 02.01.20.
//

#ifndef MATADOR_MYSQL_BOOL_HPP
#define MATADOR_MYSQL_BOOL_HPP

#if !MARIADB_PACKAGE_VERSION_ID && MYSQL_VERSION_ID >= 80001
typedef bool my_bool;
#endif

#endif //MATADOR_MYSQL_BOOL_HPP
