#ifndef MATADOR_THREAD_HELPER_HPP
#define MATADOR_THREAD_HELPER_HPP

#include "matador/utils/export.hpp"

#include <thread>

namespace matador {

OOS_UTILS_API std::size_t acquire_thread_index(std::thread::id id);

}

#endif //MATADOR_THREAD_HELPER_HPP
