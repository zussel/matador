//
// Created by sascha on 30.09.15.
//

#include "object/identifier.hpp"
namespace oos {

template<>
std::type_index identifier<std::string>::type_index_ = std::type_index(typeid(identifier<std::string>));

}