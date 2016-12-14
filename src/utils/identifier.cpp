//
// Created by sascha on 30.09.15.
//

#include "oos/utils/identifier.hpp"

namespace oos {

std::type_index identifier<std::string>::type_index_ = std::type_index(typeid(identifier<std::string>));

}