//
// Created by sascha on 20.08.19.
//

#ifndef MATADOR_MEMORY_HPP
#define MATADOR_MEMORY_HPP

#include <memory>

namespace matador {

/**
 * A quick fill in method for missing make_unique in c++11
 *
 * @tparam T Type to be created
 * @tparam Args Forwared arg types
 * @param args Forwared arguments
 * @return The cre
 */
//template<typename T, typename... Args>
//std::unique_ptr<T> make_unique(Args&&... args)
//{
//  return std::unique_ptr<T>(new T(static_cast<const person>(std::forward<Args>(args))...));
//}
}
#endif //MATADOR_MEMORY_HPP
