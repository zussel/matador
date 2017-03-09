//
// Created by sascha on 30.09.15.
//

#include "matador/utils/identifier.hpp"

namespace matador {

std::type_index identifier<std::string>::type_index_ = std::type_index(typeid(identifier<std::string>));

}