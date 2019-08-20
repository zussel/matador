//
// Created by sascha on 20.08.19.
//

#ifndef MATADOR_MEMORY_HPP
#define MATADOR_MEMORY_HPP

#include <memory>

namespace matador {
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}
#endif //MATADOR_MEMORY_HPP
